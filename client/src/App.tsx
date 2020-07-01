import React from 'react'
import { useQuery } from 'react-query'
import { getStreams } from './http'

function App() {
  const { data, isLoading, error } = useQuery('getStreams', getStreams)
  if (isLoading) return <div>Loading...</div>
  if (!data || error) return <div>Failed...</div>

  if (!data.length) return <div>No streams available...</div>

  return (
    <div>
      {data.map(stream => {
        return (
          <video
            key={stream.id}
            autoPlay
            style={{ backgroundColor: 'gray' }}
            width={800}
            height={600}
            src={`http://localhost:3000/stream/${stream.id}/watch`}
          />
        )
      })}
    </div>
  )

  // return (
  //   <video
  //     width="800"
  //     height="600"
  //     autoPlay
  //     style={{ backgroundColor: 'gray' }}
  //     src="http://localhost:3000/test"
  //   >
  //     <source type="video/webm" />
  //   </video>
  // )
}

export default App
