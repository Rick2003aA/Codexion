### 2026/02/23
まずは何をするかを確認
### **ストーリー**

- 舞台はテーブルを囲むcodersたち
- 中央に Quantum Compiler がある。
- coders は compile → debug → refactor をひたすら繰り返す
- compile するにはUSB dongle を二本同時に挿さないといけない
- dongle は coderの人数と同数あって、席の間に置かれている（隣同士で共有）
- もし一定時間（time_to_burnout）以内に compile できなかったら coder は脱落

**→みんなで作業してるけど、compileだけは二本の鍵が必要で、その鍵が足りないと締切に間に合わずに終了してしまう**

課題の本質

- 並行処理での資源共有
- デッドロック・スタベーションを潰す設計
- スケジューリング（fifo / edf）の実装
- 時間制約付きの監視

### 2026/02/24
とりあえずそれっぽいものを作ることにした。
AIを使うときに意識したこと・使用方法：
コードは書かせず、どのような働きをする関数を作ればよいのかを聞いて、それをもとにコードを作成→間違っている部分を指摘してもらい、コードではなく言葉で修正案を提案してもらう。これを繰り返すことで動く形にしていく。

1 gettimeofdayの練習
まずはgettimeofday()を使用して経過時間を記録するプログラムを作成。
C言語の書き方を完全に忘れており、色々苦戦したものの、whileで回して時間経過を三回記録することに成功。

出力：
elapsed = 0 ms
elapsed = 100 ms
elapsed = 201 ms

2 スレッドの実装
ログが出せたので、whileを消して同じ処理をcoder_routineに移す。
Why?→現状、mainがcoderの役割を果たしているが、実際はcoderが複数存在し、mainはそれらをまとめる役割を担うため。
mainでpthread_create()を使用してスレッドを作成し、pthread_join()で終了を待つ。その際、構造体を使用して特定の情報（スタート、coder_id）を保持できるようにした。

### 2026/02/25
1 スレッドで複数人のcoderを動かしてみる
pthreadを使用してcoder一人でcoder_routine()を回すことができた。
次はcoderを二人以上用意して同時並行してcoder_routine()行う。

コード：
	t_coder		coders[2];
	pthread_t	th[2];

上記のように、構造体を複数持たせることで、並行作業を可能にする。

2 mutexを使用してcodersの出力が崩れないようにする
printfは一行まるごと一瞬で出るとは限らず、内部で少しずつ出力することがある。
例えば、出力（printf）が重なってしまった場合、以下のような出力が起きる可能性があり、それを防ぐのがmutexである。

出力（Bad）：
100 1 is comp101 2 is compiling
iling

mutexを用いた実装の流れ
coder1: lock(鍵取る) → printf → unlock(鍵返す)
coder2: lock(鍵取る) → printf → unlock(鍵返す)
→ 絶対に混ざらない


### 2026/02/26
1 mutexの実装の続き
t_simにlog_mutexという状態をもたせ、その状態をlock, unlockで変化させることで処理が一つずつ行われるようになる。
- ### 初期化
pthread_mutex_init(&sim->log_mutex, NULL);

- ### 作業の前にロックすることでprintfの重複を避ける
pthread_mutex_lock(&sim->log_mutex);
elapsed = elapsed_ms(sim);
printf("%ld %d %s\n", elapsed, coder_id, msg);
pthread_mutex_unlock(&sim->log_mutex);

-  ### 後片付け（内部リソースの開放）
pthread_mutex_destroy(&sim->log_mutex)

ここまでの成果：複数人（pthread_create使用）でログをエラーなく処理（pthread_mutex使用）できるようになった

2 dongle(共有資源)を入れる
codersはdongleがないとcompileできずにburn outしてしまうのでdongleを奪い合う。
まずは超シンプルにする（dongle 1本）。これを二人で奪い合う構図にすれば、共有資源の挙動の最小実装ができる。
t_simにdongleの状態（lock, unlock）を持たせて、dongleが同時に利用される状態が起こらないようにする。
- ### 初期化
pthread_mutex_init(&sim->dongle, NULL);

- ### compile時にdongleをlockする → 使用後にunlock
pthread_mutex_lock(&coder->sim->dongle);
log_state(coder->sim, coder->coder_id, "got dongle");
log_state(coder->sim, coder->coder_id, "is compiling");
sleep_ms(200);
log_state(coder->sim, coder->coder_id, "released dongle");
pthread_mutex_unlock(&coder->sim->dongle);

- ### 後片付け 
pthread_mutex_destroy(&sim->dongle);

### 2026/02/27
1 dongleの本数を増やして複数人での取り合いを再現する
t_sim のdongleを*donglesにして、dongle_countをmallocする
→　*donglesはメモリ領域となり、dongle_count分メモリ上に連続して並ぶようになる。

コード：
	idx = coder->coder_id % coder->sim->dongle_count;

	pthread_mutex_lock(&coder->sim->dongles[idx]);
	log_state(coder->sim, coder->coder_id, "got dongle");
	log_state(coder->sim, coder->coder_id, "is compiling");
	sleep_ms(200);
	log_state(coder->sim, coder->coder_id, "released dongle");
	pthread_mutex_unlock(&coder->sim->dongles[idx]);
idxは何？：coderがdonbleにアクセスするための番号（メモリ上の位置）。coderは両隣のdongleしか使用できないため、使用できるdongleを制限する必要がある。

### 2026/02/28
1 dongleの必要本数を二本にする
coder_routineにおいてleft, right という変数を作成し、デッドロックが起きないようにそれぞれが適切にdongleを取り合うようにする
デッドロックとは？：coderたちがそれぞれ片手にdongleを持って離さない状態

デッドロックの回避方法
→ 小さい番号(idx)からlockする。

実装
	left = idx;
	right = (idx + 1) % coder->sim->dongle_count;
	first = ft_min(right, left);
	second = ft_max(right, left);
	→ first, secondの順でlockしてからcompileを実行するようにする

2 プログラムの終了条件を設定し、きれいにコードを閉じる
現状は指定された回数coder_routineをこなせばコードは終了するが、burn_outしたときは途中でコードを終わらせることになる。その際にきれいに処理を終了させたい。
t_simにstopとstop_mutexを追加して、boolでシグナル化する。mainでstopの切り替え（sim_request_stop()）を行い、mainが司令塔になるような構成にする。

3 monitorスレッドの実装
現状はmainでコードの終了条件を指定して管理しているが、監視役(monitor)を実装することでより整理されたコードとなる
流れ：monitorがシグナルを定期的にチェックしてsim_request_stop()を呼ぶ

4 monitorで止める条件をcoderのcompile状況を見て決める

