const express = require('express')
const app = express();
const port = 10418;

app.get("/esp_data", function(req, res){
  console.log(req.query);
  res.send("OK");
})

app.listen(port);
