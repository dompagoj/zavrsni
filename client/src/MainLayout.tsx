import React, { useContext } from 'react'
import { Flex, Box } from 'reflexbox'
import { Route, Switch, Redirect, RouteChildrenProps } from 'react-router-dom'
import { Routes } from './routes'
import { StreamsScreen } from './screens/StreamsScreen'
import { Sidebar } from './components/Sidebar'
import { Constants } from './constants'
import { UserStoreContext } from './stores/user-store'
import { useEffectOnce } from './custom-hooks'

export const MainLayout: React.FC<RouteChildrenProps> = props => {
  const userStore = useContext(UserStoreContext)

  useEffectOnce(() => {
    if (userStore.isLoggedIn) return
    userStore.getLoggedInUser().then(success => {
      if (!success) userStore.logout(props.history)
    })
  })

  return (
    <Flex height="100%">
      <Sidebar />
      <Box marginY="25px" marginLeft={Constants.SIDEBAR_WIDTH + 45} marginRight="45px" width="100%">
        <Switch>
          <Route exact path={Routes.STREAMS} component={StreamsScreen} />
          <Route exact path={Routes.PROFILE} render={() => <div>Profile</div>} />
          <Redirect to={Routes.STREAMS} />
        </Switch>
      </Box>
    </Flex>
  )
}
