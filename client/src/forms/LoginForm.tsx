import React from 'react'
import { Input, Button, Form } from 'antd'
import { UserOutlined, LockOutlined } from '@ant-design/icons'
import { ILoginForm } from '../types/forms'
import { FormInstance } from 'antd/lib/form'

interface IProps {
  form: FormInstance
  onSubmit: (values: ILoginForm) => void
}

export const LoginForm: React.FC<IProps> = ({ onSubmit }) => {
  return (
    <Form
      // @ts-ignore
      onFinish={onSubmit}
    >
      <Form.Item name="username" rules={[{ required: true, message: 'Username is required' }]}>
        <Input placeholder="Username" prefix={<UserOutlined />} />
      </Form.Item>
      <Form.Item name="password" rules={[{ required: true, message: 'Password is required' }]}>
        <Input type="password" placeholder="Password" prefix={<LockOutlined />} />
      </Form.Item>
      <Button style={{ marginTop: 5 }} type="primary" htmlType="submit">
        Enter
      </Button>
    </Form>
  )
}
