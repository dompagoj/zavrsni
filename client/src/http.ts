import { axios } from './axios'
import { IStream } from './types/http'
import { ILoginForm } from './types/forms'

export async function getStreams(): Promise<IStream[]> {
  const { data } = await axios.get('/streams')

  return data
}

export async function login(form: ILoginForm) {
  const { data } = await axios.post('/auth/login', form)

  return data
}

export async function me() {
  const { data } = await axios.get('/auth/me')

  return data
}
