import { Router } from 'express'
import { UNAUTHORIZED, BAD_REQUEST, OK } from 'http-status-codes'

import { streamingClients } from '../tcp-server'
import { isLoggedIn } from '../middlewares'
import { verifyTokenFromQueryParams } from '../utils'
import { createReadStream } from 'fs'

export const streamsRouter = Router()

streamsRouter.get('/', isLoggedIn, (req, res) => {
  return res.status(OK).json(streamingClients.getClients())
})

streamsRouter.get('/:streamId/watch', async (req, res) => {
  if (!(await verifyTokenFromQueryParams(req))) return res.status(UNAUTHORIZED).end()

  const { streamId } = req.params
  const streamingClient = streamingClients.getClient(streamId)
  if (!streamingClient) return res.status(BAD_REQUEST).end()

  res.setHeader('Content-Type', 'video/webm')

  res.write(streamingClient.header)
  streamingClient.socket.pipe(res)
})

streamsRouter.get('/test', (req, res) => {
  const fileStream = createReadStream('./test.webm')

  res.setHeader('Content-Type', 'video/webm')

  fileStream.pipe(res)
})
