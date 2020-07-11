import React from 'react'

export const Title: React.FC = props => {
  return (
    <div>
      <span className="title">{props.children}</span>
    </div>
  )
}
