import React from 'react'
import ReactDOM from 'react-dom'
import { ReactQueryConfigProvider } from 'react-query'
import { BrowserRouter } from 'react-router-dom'

import './index.css'
import 'antd/dist/antd.css'
import * as serviceWorker from './serviceWorker'
import { QueryConfig } from './query-config'
import { MainRouter } from './main-router'

ReactDOM.render(
  // <React.StrictMode>
  <ReactQueryConfigProvider config={QueryConfig}>
    <BrowserRouter>
      <MainRouter />
    </BrowserRouter>
  </ReactQueryConfigProvider>,
  // </React.StrictMode>,
  document.getElementById('root')
)

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister()
