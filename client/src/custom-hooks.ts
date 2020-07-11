import { useEffect, useRef, useState } from 'react'

export function useEffectOnce(callback: () => any) {
  useEffect(callback, [])
}

export function useIsVideoReady() {
  const videoRef = useRef<HTMLVideoElement>(null)

  const [loading, setLoading] = useState(true)
  const [failed, setFailed] = useState(false)

  useEffect(() => {
    const setFailedTimeout = setTimeout(() => {
      setFailed(true)
      setLoading(false)
      clearInterval(isReadyInterval)
    }, 10000)

    const isReadyInterval = setInterval(() => {
      const isReady = (videoRef.current && videoRef.current?.readyState >= 3) ?? false
      if (isReady) {
        setLoading(false)
        clearTimeout(setFailedTimeout)
        clearInterval(isReadyInterval)
      }
    }, 250)
  }, [videoRef])

  return {
    videoRef,
    loading,
    failed,
  }
}
