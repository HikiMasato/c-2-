DungeonRPG

バンタンゲームアカデミー東京校
2年生ゲームプログラマー専攻１年次

名前 : 比氣　聖斗(19)


<ゲームジャンル>
・ローグライク
・ダンジョンRPG

<ゲームの大まかな流れ>
・階層を移動するごとに敵が強くなっていくので、敵を倒すことでドロップするアイテムを入手したり、自身
　のステータスを強化するアイテムをショップで買って使ったり、マップ内にランダムにドロップしてあるア
　イテムを駆使しながら、自身の所有するスキルをセットして様々なスキルを発動し、効果力で敵を圧倒し、
　自身を強化しながらレベルを上げ、より深い階層を目指していく。


<ゲームとしてこだわった部分>
・ランダムに重きを置いた。マップの生成方法、敵、アイテムのランダムスポーン、ランダムなステータス強
　化値、などゲーム内におけるほとんどの機能をランダムで実装したため、遊ぶたびに毎回違うパターンとな
　るのでゲームの難易度などが全く違うものとなっている。

・敵のステータスはキャラごとに統一してあるが、攻撃をランダムにしてくるので毎回同じ戦闘にならず長く
　楽しみやすい。

・ほかのローグライクのゲームのようにターン制ではなくリアルタイムバトルなので敵が迫ってくる緊迫感、
　瞬時の判断力、敵から逃げるときの焦燥感に駆られる。これによって、ゲームへの没入感を少しでも高めら
　れるようにした

・ドラクエⅨのように宝の地図で洞窟の探検をする際の　地下へ続く階段を探すドキドキ、わくわく感を出し
　たかったので、ダンジョン内のBGMを、オマージュしたものにしたり、階段の設置位置をランダムに配置する
　ようにした。

<プログラムの観点で工夫した部分>
・マップのランダム生成(区域分割法、穴掘り法)
・区域分割法のみでは、一方通行になってしまい、面白みが出ないと思い、穴掘り法で通路を追加して作ること
　により、マップに面白みを持たせた。
・キャラデータ、画像ハンドルは、ほぼすべてcsvから取得し使用している。
・敵の行動を経路探索による移動にした。(ダイクストラ法、A*法)
・今後キャラ、エフェクト、マップの種類を追加するときは、csvファイルにデータを書くだけでほとんど追加できるようになっている。
・メルセンヌツイスターを使用することで、偏りの少ない高度なランダムを表現している。


<実装していっている機能>
・マップのランダム生成(区域分割法、穴掘り法)
・インベントリ
・敵Ai行動処理(ダイクストラ法、A*法)
・敵のランダム生成(ポジション、種類)
・メニュー設定(WIP)
・ミニマップ
・ドラッグ&ドロップ
・ショップ


<著作権>
カーソル画像 : <a target="_blank" href="https://icons8.com/icon/Do2-6JX07Pe6/minecraft%E3%81%AE%E5%89%A3">Minecraftの剣</a> アイコン by <a target="_blank" href="https://icons8.com">Icons8</a>