import { Router } from 'express'
import { streamsRouter } from './streams-router'
import { authRouter } from './auth-router'

export const mainRouter = Router()

mainRouter.use('/streams', streamsRouter)

mainRouter.use('/auth', authRouter)
