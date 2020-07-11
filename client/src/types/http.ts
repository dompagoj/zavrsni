import { IUser } from "./data";

export interface IStream {
  id: string
}

export interface ILogginResponse {
  token: string
  user: IUser
}