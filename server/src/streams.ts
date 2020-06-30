import { Socket } from "net";

class CameraStream {
  constructor(public header: Buffer, public socket: Socket) { }
}

export class StreamingClients {
  private clients: { [id: string]: CameraStream } = {}

  public getClients() {
    return Object.values(this.clients).map(({ socket }) => ({
      // @ts-ignore
      id: socket.id,
    }))
  }

  public getClient(clientId: string) {
    return this.clients[clientId]
  }

  public addClient(header: Buffer, socket: Socket) {
    // @ts-ignore
    this.clients[socket.id] = new CameraStream(header, socket)
  }

  public removeClient(socket: Socket) {
    // @ts-ignore
    delete this.clients[socket.id]
  }

}