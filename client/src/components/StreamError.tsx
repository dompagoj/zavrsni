import React from 'react'
import { Flex, Box } from 'reflexbox'
import { ExclamationCircleOutlined } from '@ant-design/icons'

interface Props {
  msg?: string
}

export const StreamError: React.FC<Props> = props => {
  return (
    <Flex flexDirection="column">
      <Box marginBottom={1} color="white" fontSize={24}>
        {props.msg}
      </Box>
      <ExclamationCircleOutlined style={{ color: 'red', fontSize: 100 }} />
    </Flex>
  )
}
