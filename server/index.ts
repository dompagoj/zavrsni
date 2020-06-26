process.title = 'Transcoder server'

import Dgram from 'dgram'
import cors from 'cors'
import { createServer, Socket } from 'net'
import Express from 'express'
import { PassThrough } from 'stream'
import { createReadStream, createWriteStream, unlink, existsSync, unlinkSync } from 'fs'
import { promisify } from 'util'

// const filename = './test.mp4'
// if (existsSync(filename)) unlinkSync(filename)
// const fileStream = createWriteStream(filename)

// let header: Buffer

const TCPServer = createServer()
const HTTPServer = Express()

const UDP_PORT = 13000
const HTTP_PORT = 3000
const ADDRESS = 'localhost'

const passThrough = new PassThrough({ emitClose: false, defaultencoding: 'binary' })

const CLIENTS: { [key: string]: Socket } = {}

TCPServer.on('connection', socket => {
  console.log('Got connection! ', socket.localAddress)
  CLIENTS[socket.localAddress] = socket

  socket.on('data', msg => {
    // if (!header) return header = msg
    passThrough.write(msg)
    passThrough.resume()
  })

  socket.on('close', () => {
    console.log('Socket client disconnect')
    delete CLIENTS[socket.localAddress]
  })
})

TCPServer.listen(UDP_PORT, () => {
  console.log(`Tcp server listening on port ${UDP_PORT} at ${ADDRESS}`)
})

HTTPServer.use(cors())

HTTPServer.get('/stream', async (req, res) => {
  res.setHeader('Content-Type', 'octet-stream')
  // res.setHeader('Content-Type', 'video/mp4')
  res.setHeader('Transfer-Encoding', 'chunked')

  // header && res.write(header, 'binary')
  passThrough.on('data', data => {
    res.write(data, 'binary')
  })
})

HTTPServer.listen(HTTP_PORT, () => {
  console.log(`Http server listening on ${HTTP_PORT}`)
})