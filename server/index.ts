process.title = 'Transcoder server'

import Dgram from 'dgram'
import Express from 'express'
import { PassThrough } from 'stream'
import { createReadStream, createWriteStream } from 'fs'

const UDPServer = Dgram.createSocket('udp4')
const HTTPServer = Express()

const UDP_PORT = 13000
const HTTP_PORT = 3000
const ADDRESS = 'localhost'

const passThrough = new PassThrough()
let Header: Buffer


UDPServer.on('message', (msg, info) => {
  if (!Header) return Header = msg

  passThrough.write(msg)
  passThrough.resume()
})

UDPServer.bind(UDP_PORT, ADDRESS, () => {
  console.log(`Udp server listening on port ${UDP_PORT} at ${ADDRESS}`)
})



HTTPServer.get('/', (req, res) => {
  res.setHeader('Content-Type', 'video/mp4')

  res.write(Header)
  passThrough.pipe(res)
})

HTTPServer.listen(HTTP_PORT, () => {
  console.log(`Http server listening on ${HTTP_PORT}`)
})