import React, { useState, useEffect } from 'react'

function App() {
  const [showPlayer, setShowPlayer] = useState(false)

  // useEffect(() => {
  //   fetch('http://localhost:3000/stream')
  //     .then(response => response.body)
  //     .then(async body => {
  //       const reader = body?.getReader()
  //       if (!reader) return
  //       let finished = false

  //       while (!finished) {
  //         const { done, value } = await reader?.read()
  //         finished = done

  //         console.log('Value: ', value)
  //       }
  //     })
  // }, [])

  return (
    <div>
      <button onClick={() => setShowPlayer(!showPlayer)}>
        {showPlayer ? 'Stop streaming' : 'Start the stream'}
      </button>

      {showPlayer && (
        <video
          autoPlay
          style={{ backgroundColor: 'gray' }}
          width={800}
          height={600}
          src="http://localhost:3000/stream"
        />
      )}
    </div>
  )
}

export default App
