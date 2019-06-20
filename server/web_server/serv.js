const express = require('express')
const app = express();
const port = 10418;

app.get("/data", function(req, res){
  console.log(req.query.esp_data);
  res.send("OK");
})

app.listen(port);
