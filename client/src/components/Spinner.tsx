import React, { CSSProperties } from 'react'
import { LoadingOutlined } from '@ant-design/icons'
import { Box } from 'reflexbox'
import { Spin } from 'antd'

interface IProps {
  loading?: boolean
  style?: CSSProperties
  size?: number
}

export const Spinner: React.FC<IProps> = props => {
  if (!props.loading) return <></>

  return (
    <Box style={props.style}>
      <Spin
        size="large"
        spinning={props.loading}
        indicator={<LoadingOutlined style={{ fontSize: props.size ?? 60, color: 'white' }} spin />}
      />
    </Box>
  )
}
