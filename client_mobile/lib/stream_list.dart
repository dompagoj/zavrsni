import 'package:client_mobile/models/stream.dart';
import 'package:client_mobile/stream.dart';
import 'package:flutter/material.dart';

class StreamList extends StatelessWidget {
  final List<StreamModel> streams;

  StreamList({@required this.streams});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: streams.isEmpty
          ? Text(
              'No streams available',
              style: TextStyle(fontWeight: FontWeight.bold, fontSize: 21),
            )
          : ListView.builder(
              shrinkWrap: true,
              itemCount: streams.length,
              itemBuilder: (_, i) {
                return Align(
                  child: MyStream(
                    streamId: streams[i].id,
                  ),
                );
              },
            ),
    );
  }
}
