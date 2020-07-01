process.title = 'Transcoder server'

import cors from 'cors'
import Express from 'express'
import { TCPServer } from './tcp-server'
import { MainRouter } from './main-router'


const HTTPServer = Express()
const HTTP_PORT = 3000
const TCP_PORT = 13000
const ADDRESS = 'localhost'

TCPServer.listen(TCP_PORT, () => {
  console.log(`Tcp server listening on port ${TCP_PORT} at ${ADDRESS}`)
})

HTTPServer.use(cors())
HTTPServer.use(MainRouter)

HTTPServer.listen(HTTP_PORT, () => {
  console.log(`Http server listening on ${HTTP_PORT}`)
})