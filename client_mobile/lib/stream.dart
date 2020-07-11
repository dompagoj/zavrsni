import 'package:flutter/material.dart';
import 'package:video_player/video_player.dart';

class MyStream extends StatefulWidget {
  final String streamId;

  MyStream({Key key, @required this.streamId}) : super(key: key) {
    assert(streamId != null);
  }

  @override
  _MyStreamState createState() => _MyStreamState();
}

class _MyStreamState extends State<MyStream> {
  VideoPlayerController _controller;

  @override
  void initState() {
    super.initState();
    // _controller = VideoPlayerController.network('http://10.0.2.2:3000/streams/${widget.streamId}/watch')
    _controller = VideoPlayerController.network('http://10.0.2.2:3000/streams/test')
      ..initialize().then((value) {
        _controller.play();
        setState(() {});
      });
  }

  @override
  void dispose() {
    super.dispose();
    _controller.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return _controller.value.initialized
        ? Container(
            height: 75,
            width: 75,
            child: CircularProgressIndicator(),
          )
        : Container(
            height: 250,
            child: VideoPlayer(_controller),
          );
  }
}
