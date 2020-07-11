import 'package:flutter/material.dart';

class MyAppBar extends StatelessWidget implements PreferredSizeWidget {
  final String title;

  const MyAppBar({this.title = 'Home security system'});

  @override
  Widget build(BuildContext context) {
    return AppBar(
      title: Text(title),
      backgroundColor: Colors.black54,
    );
  }

  @override
  Size get preferredSize => Size.fromHeight(50);
}
