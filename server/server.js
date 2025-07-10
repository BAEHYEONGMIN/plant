const express = require("express");
const app = express();
let latestCommand = null;
// 메인 페이지 (홈 페이지) 설정
app.get("/", (req, res) => {
  res.send(`
    <html>
      <head>
        <title>서버 상태</title>
      </head>
      <body>
        <h1>서버가 정상적으로 실행 중입니다!</h1>
        <p>현재 LED 상태: <strong>${latestCommand ?? "없음"}</strong></p>
        <p>LED 상태를 제어하려면 <a href="/led/on">LED ON</a> 또는 <a href="/led/off">LED OFF</a>를 클릭하세요. <--테스트용임. 작동안함.</p>
        <a href="/set-command?cmd=on">LED ON</a><br>
        <a href="/set-command?cmd=off">LED OFF</a>
      </body>
    </html>
  `);
});

// 간단한 LED 제어 API
app.get("/led/on", (req, res) => {
  console.log("LED ON 요청 받음");
  res.send("{len_on}");
});
app.get("/ping", (req, res) => {
  console.log("ping-pong");
  res.send("{pong}");
});
app.get("/set-command", (req, res) => {
  const cmd = req.query.cmd;
  if (cmd === "on" || cmd === "off") {
    latestCommand = cmd;
    console.log("명령 저장됨:", cmd);
    //res.send("명령 저장됨: " + cmd);
    res.send(`
    <html>
      <head>
        <meta http-equiv="refresh" content="3;url=/" />
        <title>명령 처리</title>
      </head>
      <body>
        <h2>명령 처리됨: ${cmd}</h2>
        <p>3초 후 메인 페이지로 이동합니다...</p>
        <p><a href="/">지금 바로 이동</a></p>
      </body>
    </html>
  `);
  } else {
    res.send("잘못된 명령입니다.");
  }
  
});
app.get("/get-command", (req, res) => {
  if (latestCommand) {
    res.send("{led_"+latestCommand+"}");
    console.log("ESP에 명령 전달:", latestCommand);
    latestCommand = null; // 한 번 주면 초기화
  } else {
    res.send("{led_"+latestCommand+"}");
  }
});
app.use((req, res, next) => {
  console.log("요청 URL:", req.url);
  next();
});
// Glitch에서는 process.env.PORT를 사용하여 포트를 설정
app.listen(process.env.PORT || 3000, () => {
  console.log("서버 실행 중");
});
