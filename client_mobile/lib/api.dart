import 'package:client_mobile/models/stream.dart';
import 'package:dio/dio.dart';

class Api {
  final Dio _dio = Dio(
    BaseOptions(baseUrl: 'http://10.0.2.2:3000', headers: {
      'Authorization':
          'bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6MSwidXNlcm5hbWUiOiJkb21wYWdvaiIsImlhdCI6MTU5NDQwOTQ4OSwiZXhwIjoxNTk5NTkzNDg5fQ.mKo-VBMPY7yoqZl_vkHtUtDQ5pulunimYeYDNFRT_bQ'
    }),
  );

  Future<List<StreamModel>> getStreams() async {
    final response = await _dio.get('/streams');

    return StreamModel.fromJsonList(response.data);
  }
}

final api = Api();
