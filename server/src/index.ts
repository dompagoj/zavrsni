require('dotenv').config()
import cors from 'cors'
import Express from 'express'
import { TCPServer } from './tcp-server'
import { mainRouter } from './routers/main-router'
import { createConnection } from 'typeorm'
import { seedUsers, verifyEnvVariables } from './utils'
import bodyParser from 'body-parser'

const HTTPServer = Express()
const HTTP_PORT = 3000
const TCP_PORT = 13000
const ADDRESS = 'localhost'

async function main() {
  verifyEnvVariables()
  await createConnection()
  await seedUsers()

  TCPServer.listen(TCP_PORT, () => {
    console.log(`Tcp server listening on port ${TCP_PORT} at ${ADDRESS}`)
  })

  HTTPServer.use(bodyParser.json())
  HTTPServer.use(cors())
  HTTPServer.use(mainRouter)

  HTTPServer.listen(HTTP_PORT, () => {
    console.log(`Http server listening on ${HTTP_PORT}`)
  })
}

main().catch(console.error)
