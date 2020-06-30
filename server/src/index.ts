process.title = 'Transcoder server'

import cors from 'cors'
import { createServer, Socket } from 'net'
import Express from 'express'
import { promisify } from 'util'
import { join } from 'path'

const sleep = promisify(setTimeout)

const TCPServer = createServer()
const HTTPServer = Express()

const UDP_PORT = 13000
const HTTP_PORT = 3000
const ADDRESS = 'localhost'

let HEADER: Buffer
const CLIENTS: { [key: string]: Socket } = {}

TCPServer.on('connection', socket => {
  CLIENTS['test'] = socket

  socket.on('data', data => {
    HEADER = data
    socket.removeAllListeners()
  })

})

TCPServer.listen(UDP_PORT, () => {
  console.log(`Tcp server listening on port ${UDP_PORT} at ${ADDRESS}`)
})

HTTPServer.use(cors())


HTTPServer.use('/app', Express.static(join(__dirname, '../react-app')))
HTTPServer.use('/static', Express.static(join(__dirname, '../react-app/static')))

HTTPServer.get('/stream', async (req, res) => {
  if (!CLIENTS['test']) await sleep(2500)
  console.log('Header length: ', HEADER.length)
  res.setHeader('Content-Type', 'octet-stream')
  res.setHeader('Transfer-Encoding', 'chunked')

  req.on('close', () => {
    res.status(200).end()
  })

  res.write(HEADER)
  CLIENTS['test'].pipe(res)
})

HTTPServer.listen(HTTP_PORT, () => {
  console.log(`Http server listening on ${HTTP_PORT}`)
})





// process.title = 'Transcoder server'

// import cors from 'cors'
// import Express from 'express'
// import { TCPServer } from './tcp-server'
// import { MainRouter } from './main-router'


// const HTTPServer = Express()
// const HTTP_PORT = 3000
// const TCP_PORT = 13000
// const ADDRESS = 'localhost'

// TCPServer.listen(TCP_PORT, () => {
//   console.log(`Tcp server listening on port ${TCP_PORT} at ${ADDRESS}`)
// })

// HTTPServer.use(cors())
// HTTPServer.use(MainRouter)

// HTTPServer.listen(HTTP_PORT, () => {
//   console.log(`Http server listening on ${HTTP_PORT}`)
// })