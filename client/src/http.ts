import { axios } from "./axios";
import { Stream } from "stream";
import { IStream } from "./types";

export async function getStreams(): Promise<IStream[]> {
  const { data } = await axios.get('streams')

  return data
}

export async function stopStream() {

}