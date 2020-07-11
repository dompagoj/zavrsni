import 'package:client_mobile/api.dart';
import 'package:client_mobile/appbar.dart';
import 'package:client_mobile/models/stream.dart';
import 'package:client_mobile/stream_list.dart';
import 'package:flutter/material.dart';

class HomeScreen extends StatefulWidget {
  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  Future<List<StreamModel>> _streamsFuture;

  @override
  void initState() {
    super.initState();

    setState(() {
      _streamsFuture = api.getStreams();
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: MyAppBar(),
      body: FutureBuilder<List<StreamModel>>(
        future: _streamsFuture,
        builder: (_, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) return Center(child: CircularProgressIndicator());

          if (snapshot.hasError) return Center(child: Text(snapshot.error.toString()));

          return StreamList(
            streams: snapshot.data,
          );
        },
      ),
    );
  }
}
