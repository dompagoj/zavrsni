import { observable, action } from 'mobx'
import { createContext } from 'react'
import { History } from 'history'

import { IUser } from '../types/data'
import { Routes } from '../routes'
import { axios } from '../axios'
import * as Http from '../http'

export class UserStore {
  @observable user!: IUser
  @observable token?: string

  get isLoggedIn() {
    return !!this.user
  }

  @action
  setUser = (user: IUser) => {
    this.user = user
  }

  @action setToken(token: string) {
    axios.defaults.headers.authorization = 'bearer ' + token
    this.token = token
  }

  @action
  getLoggedInUser = async () => {
    try {
      const { user } = await Http.me()
      this.user = user

      return true
    } catch (e) {
      return false
    }
  }

  @action logout = (history?: History) => {
    // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
    this.user = undefined!
    localStorage.removeItem('token')
    history?.push(Routes.LOGIN)
  }
}

export const userStore = new UserStore()
export const UserStoreContext = createContext(userStore)
