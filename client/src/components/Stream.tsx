import React, { useRef, useEffect, useState, useContext } from 'react'
import { Box, Flex } from 'reflexbox'

import { EntityID } from '../types/data'
import { Spinner } from './Spinner'
import { StreamError } from './StreamError'
import { useIsVideoReady } from '../custom-hooks'
import { UserStoreContext } from '../stores/user-store'

interface Props {
  id: EntityID
  title: string
}

export const Stream: React.FC<Props> = ({ id, title }) => {
  const userStore = useContext(UserStoreContext)
  const { videoRef, failed, loading } = useIsVideoReady()

  const streamUrl = `${process.env.REACT_APP_API_URL}streams/${id}/watch?token=${userStore.token}`

  return (
    <Flex flexDirection="column">
      <Box textAlign="center" className="stream-title">
        {title}
      </Box>
      <Box style={{ position: 'relative' }}>
        <Flex style={{ position: 'absolute' }} width="100%" height="100%" justifyContent="center" alignItems="center">
          {loading && <Spinner size={200} loading />}
          {failed && <StreamError msg={`${title} has failed to load`} />}
        </Flex>
        <video
          ref={videoRef}
          autoPlay
          style={{ backgroundColor: 'gray' }}
          width={800}
          height={600}
          className="stream-wrapper"
        >
          <source src={streamUrl} type="video/webm" />
        </video>
      </Box>
    </Flex>
  )
}
