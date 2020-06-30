import { Router } from "express";
import { streamingClients } from "./tcp-server";

export const MainRouter = Router()

MainRouter.get('/streams', (req, res) => {
  const clients = streamingClients.getClients()

  return res.status(200).json(clients)
})

MainRouter.get('/stream/:streamId/watch', async (req, res) => {
  const { streamId } = req.params
  const streamingClient = streamingClients.getClient(streamId)
  if (!streamingClient) return res.status(400).end()

  res.setHeader('Content-Type', 'octet-stream')
  res.setHeader('Transfer-Encoding', 'chunked')

  res.write(streamingClient.header)
  streamingClient.socket.pipe(res)
})
