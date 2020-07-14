import bcrypt from 'bcryptjs'
import { readFileSync } from 'fs'
import { join } from 'path'
import jwt from 'jsonwebtoken'

import { User } from './entities/user'
import { Config } from './config'
import { IJwtPayload as IJWTPayload, IJwtPayload } from './types'
import { Response, Request } from 'express'

export function verifyEnvVariables() {
  Object.entries(Config).forEach(([key, value]) => {
    if (!value) {
      console.log(`ENV CHECK: [${key}] has no value!`)
    }
  })
}

export function hashPassword(password: string) {
  return bcrypt.hash(password, 8)
}

export async function comparePasswords(password: string, passwordHash: string) {
  return bcrypt.compare(password, passwordHash)
}

export async function seedUsers() {
  if ((await User.count()) !== 0) return

  const users = JSON.parse(readFileSync(join(__dirname, '..', 'users.json')).toString()) as User[]

  return users.map(async ({ username, password }) => {
    return User.create({
      username,
      password: await hashPassword(password),
    }).save()
  })
}

export function generateJWT(user: User) {
  return jwt.sign(
    {
      id: user.id,
      username: user.username,
    },
    Config.JWT_SIGN_KEY,
    { expiresIn: '60 days' }
  )
}

export function verifyJWT(token: string) {
  return new Promise<IJWTPayload | undefined>((resolve, reject) => {
    jwt.verify(token, Config.JWT_SIGN_KEY, (err, user) => {
      if (err) return resolve(undefined)

      return resolve(user as IJWTPayload)
    })
  })
}

export function parseAuthorizationHeader(header?: string) {
  if (!header) return

  const [prefix, token] = header.split(' ')
  if (!prefix || prefix !== 'bearer' || !token) return

  return token
}

export function getUserFromLocals(res: Response): IJwtPayload {
  return res.locals.user
}

export function verifyTokenFromQueryParams(req: Request) {
  const { token } = req.query
  if (!token || typeof token !== 'string') return

  return verifyJWT(token)
}
