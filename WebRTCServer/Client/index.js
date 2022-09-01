const express = require('express')
const path = require('path');

const app = express()
const port = 3000

const rootPath = "/views"

app.use(express.static(path.join(__dirname, 'views')));

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '/example1/index.html'));
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})