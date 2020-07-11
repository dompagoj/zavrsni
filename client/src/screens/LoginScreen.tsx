import React, { useContext } from 'react'
import { LoginForm } from '../forms/LoginForm'
import { Form } from 'antd'
import { ILoginForm } from '../types/forms'
import * as Http from '../http'
import { UserStoreContext } from '../stores/user-store'
import { RouteChildrenProps } from 'react-router-dom'
import { Routes } from '../routes'

export const LoginScreen: React.FC<RouteChildrenProps> = props => {
  const [form] = Form.useForm()
  const userStore = useContext(UserStoreContext)

  const onSubmit = async (form: ILoginForm) => {
    const { user, token } = await Http.login(form)
    userStore.setUser(user)
    userStore.setToken(token)
    localStorage.setItem('token', token)

    props.history.push(Routes.STREAMS)
  }

  return (
    <div className="login-wrapper">
      <h2 className="login-text">Home Security System Login</h2>
      <div className="login-form">
        <LoginForm form={form} onSubmit={onSubmit} />
      </div>
    </div>
  )
}
