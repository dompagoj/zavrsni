import React, { useState, useContext } from 'react'
import { Switch, Route, Redirect } from 'react-router-dom'
import { observer } from 'mobx-react-lite'
import { MainLayout } from './MainLayout'
import { Routes } from './routes'
import { UserStoreContext } from './stores/user-store'
import { useEffectOnce } from './custom-hooks'
import { LoginScreen } from './screens/LoginScreen'

export const MainRouter = observer(() => {
  const [loading, setLoading] = useState(true)
  const userStore = useContext(UserStoreContext)

  useEffectOnce(() => {
    const token = localStorage.getItem('token')
    if (token) userStore.setToken(token)

    setLoading(false)
  })

  if (loading) return <div></div>

  return (
    <Switch>
      <Route exact path={Routes.LOGIN} component={LoginScreen} />
      {!userStore.token && <Redirect to={Routes.LOGIN} />}
      <Route path={Routes.HOME} component={MainLayout} />
      <Redirect to={Routes.HOME} />
    </Switch>
  )
})
