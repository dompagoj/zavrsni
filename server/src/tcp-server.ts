import { createServer } from 'net'
import { StreamingClients } from './streams'
import { uuid } from 'uuidv4'
import { createWriteStream } from 'fs'

export const TCPServer = createServer()
export const streamingClients = new StreamingClients()

TCPServer.on('connection', socket => {
  // const fileStream = createWriteStream('./test.webm')

  // socket.pipe(fileStream)

  console.log('Streaming device connected! ', socket.localAddress, socket.localPort)
  // @ts-ignore
  socket.id = uuid()

  socket.once('data', header => {
    streamingClients.addClient(header, socket)
  })

  socket.on('close', () => {
    console.log('Close!')
    streamingClients.removeClient(socket)
  })
  socket.on('end', () => {
    console.log('End!')
    streamingClients.removeClient(socket)
  })
})