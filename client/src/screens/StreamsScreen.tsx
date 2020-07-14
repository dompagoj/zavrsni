import React, { useState } from 'react'
import { useQuery, queryCache } from 'react-query'

import { getStreams } from '../http'
import { Flex, Box } from 'reflexbox'
import { Title } from '../components/Title'
import { Stream } from '../components/Stream'
import { Button, message } from 'antd'
import { IStream } from '../types/http'

const streams = [1, 2, 3, 4]

export const StreamsScreen: React.FC = () => {
  const { data, isLoading, error } = useQuery('getStreams', getStreams)
  if (isLoading) return <div>Loading...</div>
  if (!data || error) return <div>Failed...</div>

  const refresh = async () => {
    const [newData] = await queryCache.invalidateQueries('getStreams')
    if (newData.length === data.length) message.info('No new streams found')
  }

  if (!data.length)
    return (
      <Flex marginTop={5} fontWeight="bold" fontSize={24} width="100%" alignItems="center" flexDirection="column">
        <Box>No streams available...</Box>
        <Box marginTop={2}>
          <Button onClick={refresh} type="primary">
            Refresh
          </Button>
        </Box>
      </Flex>
    )

  return (
    <Flex flexDirection="column">
      <Title>Streams</Title>
      <Flex flexWrap="wrap" marginTop={5} justifyContent="space-evenly">
        {data.map((stream, index) => {
          return (
            <Box key={stream.id} width={800} height={600} marginBottom={5}>
              <Stream title={`Streaming device (${index + 1})`} id={stream.id} />
            </Box>
          )
        })}
      </Flex>
    </Flex>
  )
}
