import { createServer } from 'net'
import { StreamingClients } from './streams'
import { uuid } from 'uuidv4'
import { createWriteStream } from 'fs'

export const TCPServer = createServer()
export const streamingClients = new StreamingClients()

TCPServer.on('connection', socket => {
  console.log('Streaming device connected! ', socket.localAddress, socket.localPort)
  // @ts-ignore
  socket.id = uuid()

  socket.once('data', header => {
    streamingClients.addClient(header, socket)
  })

  socket.on('close', () => {
    // @ts-ignore
    console.log(`Streaming device ${socket.id} disconnected!`)
    streamingClients.removeClient(socket)
  })
  socket.on('end', () => {
    // @ts-ignore
    console.log(`Streaming device ${socket.id} disconnected!`)
    streamingClients.removeClient(socket)
  })
})
