import { RequestHandler } from 'express'
import { parseAuthorizationHeader, verifyJWT } from './utils'
import { UNAUTHORIZED } from 'http-status-codes'

export const isLoggedIn: RequestHandler = async (req, res, next) => {
  const token = parseAuthorizationHeader(req.headers.authorization)

  if (!token) return res.status(UNAUTHORIZED).end()

  const user = await verifyJWT(token)

  if (!user) return res.status(UNAUTHORIZED).end()

  res.locals.user = user

  next()
}
