import { Router } from "express";
import { streamingClients } from "./tcp-server";

export const MainRouter = Router()

MainRouter.get('/streams', (req, res) => {
  return res.status(200).json(streamingClients.getClients())
})

MainRouter.get('/stream/:streamId/watch', async (req, res) => {
  const { streamId } = req.params
  const streamingClient = streamingClients.getClient(streamId)
  if (!streamingClient) return res.status(400).end()

  res.setHeader('Content-Type', 'video/webm')
  // res.setHeader('Transfer-Encoding', 'chunked')
  // res.setHeader('Connection', 'close')
  res.setHeader('Date', new Date().toUTCString())

  res.write(streamingClient.header)
  streamingClient.socket.pipe(res)
})
