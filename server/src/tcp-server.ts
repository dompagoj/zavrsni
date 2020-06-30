import { createServer, Socket } from 'net'
import { StreamingClients } from './streams'
import { uuid } from 'uuidv4'



export const TCPServer = createServer()
export const streamingClients = new StreamingClients()


TCPServer.on('connection', socket => {
  console.log('Streaming device connected! ', socket.localAddress, socket.localPort)
  // @ts-ignore
  socket.id = uuid()

  const dataHandler = (header: Buffer) => {
    streamingClients.addClient(header, socket)
    // socket.removeListener('data', dataHandler)
    socket.removeAllListeners()
  }

  socket.on('data', dataHandler)

  socket.on('close', () => {
    console.log('Close!')
    streamingClients.removeClient(socket)
  })
})