import { Router } from 'express'
import { generateJWT, comparePasswords } from '../utils'
import { User } from '../entities/user'
import { OK, BAD_REQUEST } from 'http-status-codes'
import { isLoggedIn } from '../middlewares'

export const authRouter = Router()

authRouter.post('/login', async (req, res) => {
  const { username, password } = req.body

  try {
    const user = await User.findOne({ where: { username } })
    if (!user) return res.status(BAD_REQUEST).json('Invalid username or password').end()

    if (!(await comparePasswords(password, user.password)))
      return res.status(BAD_REQUEST).json('Invalid username or password').end()

    const { password: _, ...rest } = user

    return res.status(OK).json({
      user: rest,
      token: await generateJWT(user),
    })
  } catch (e) {
    return res.status(BAD_REQUEST).end()
  }
})

authRouter.get('/me', isLoggedIn, async (req, res) => {
  const { user } = res.locals

  const { password, ...dbUser } = await User.findOneOrFail(user.id)

  return res.status(200).json(dbUser)
})
