import React from 'react'
import { Menu, Layout } from 'antd'
import { Flex } from 'reflexbox'
import { Routes } from '../routes'
import { VideoCameraAddOutlined, ProfileOutlined, LogoutOutlined } from '@ant-design/icons'
import { useHistory } from 'react-router-dom'
import { ClickParam } from 'antd/lib/menu'
import { Constants } from '../constants'
import { userStore } from '../stores/user-store'

export const Sidebar = () => {
  const history = useHistory()
  const currentRoute = history.location.pathname

  const onMenuClick = ({ key }: ClickParam) => {
    if (key === Routes.LOGIN) {
      userStore.logout(history)
    }
    history.push(key)
  }

  return (
    <Flex flexGrow={1} style={{ position: 'fixed' }}>
      <Layout.Sider width={Constants.SIDEBAR_WIDTH}>
        <Menu
          style={{
            minHeight: '100vh',
            display: 'flex',
            flexDirection: 'column',
            position: 'relative',
          }}
          theme="dark"
          mode="inline"
          selectedKeys={[currentRoute]}
          onClick={onMenuClick}
        >
          <Menu.Item key={Routes.STREAMS} icon={<VideoCameraAddOutlined />}>
            Streams
          </Menu.Item>
          <Menu.Item key={Routes.PROFILE} icon={<ProfileOutlined />}>
            Profile
          </Menu.Item>
          <Menu.Item key={Routes.LOGIN} icon={<LogoutOutlined />} style={{ marginTop: 'auto', marginBottom: '10px' }}>
            Logout
          </Menu.Item>
        </Menu>
      </Layout.Sider>
    </Flex>
  )
}
