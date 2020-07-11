class StreamModel {
  final String id;

  const StreamModel({this.id});

  factory StreamModel.fromJson(dynamic json) {
    return StreamModel(
      id: json['id'],
    );
  }

  static List<StreamModel> fromJsonList(List<dynamic> json) => json.map((e) => StreamModel.fromJson(e)).toList();
}
