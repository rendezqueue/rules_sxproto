def _sxproto_data_impl(ctx):
  sxproto_file = ctx.file.src

  binaryproto_file = None
  if ctx.outputs.out_binaryproto:
    binaryproto_file = ctx.outputs.out_binaryproto
  else:
    binaryproto_file = ctx.actions.declare_file(ctx.label.name + ".binaryproto")

  outfiles = [binaryproto_file]

  textproto_file = None
  if ctx.outputs.out_textproto:
    textproto_file = ctx.outputs.out_textproto
    outfiles.append(textproto_file)
  else:
    textproto_file = ctx.actions.declare_file(ctx.label.name + ".textproto")

  # Translate to textproto.
  args = ctx.actions.args()
  args.add_joined(["stdin=open_readonly", sxproto_file], join_with = ":")
  args.add_joined(["stdout=open_writeonly", textproto_file], join_with = ":")
  args.add("--")
  args.add(ctx.executable._sxproto2textproto)
  ctx.actions.run(
      executable = ctx.executable._fildespawn,
      arguments = [args],
      inputs = [sxproto_file],
      outputs = [textproto_file],
      tools = [ctx.executable._sxproto2textproto],
   )

  # Compile to binaryproto.
  args = ctx.actions.args()
  args.add_joined(["stdin=open_readonly", textproto_file], join_with = ":")
  args.add_joined(["stdout=open_writeonly", binaryproto_file], join_with = ":")
  args.add("--")
  args.add(ctx.executable._protoc)
  args.add_joined("--descriptor_set_in", ctx.files.proto_deps, join_with = ":")
  args.add("--encode")
  args.add(ctx.attr.proto_message)
  ctx.actions.run(
      executable = ctx.executable._fildespawn,
      arguments = [args],
      inputs = [textproto_file] + ctx.files.proto_deps,
      outputs = [binaryproto_file],
      tools = [ctx.executable._protoc],
   )

  if ctx.outputs.out_json:
    json_file = ctx.outputs.out_json
    outfiles.append(json_file)
    args = ctx.actions.args()
    args.add(binaryproto_file)
    args.add(json_file)
    args.add(ctx.attr.proto_message)
    args.add_all(ctx.files.proto_deps)
    ctx.actions.run(
        executable = ctx.executable._binaryproto2json,
        arguments = [args],
        inputs = [binaryproto_file] + ctx.files.proto_deps,
        outputs = [json_file],
     )

  return DefaultInfo(
      files = depset([binaryproto_file]),
      runfiles = ctx.runfiles(files = outfiles),
  )


sxproto_data = rule(
    implementation = _sxproto_data_impl,
    attrs = {
        "src": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "The .sxproto file to compile.",
        ),
        "out_binaryproto": attr.output(
            mandatory = False,
            doc = "The .binaryproto file to write.",
        ),
        "out_textproto": attr.output(
            mandatory = False,
            doc = "The .textproto file to write.",
        ),
        "out_json": attr.output(
            mandatory = False,
            doc = "The .json file to write.",
        ),
        "proto_deps": attr.label_list(
            mandatory = True,
            doc = "Names of proto_data that have the info needed to compile a message.",
        ),
        "proto_message": attr.string(
            mandatory = True,
            doc = "Type of message that the .sxproto file represents.",
        ),
        "_fildespawn": attr.label(
            default = Label("@fildesh//:fildespawn"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_protoc": attr.label(
            default = Label("@protobuf//:protoc"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_sxproto2textproto": attr.label(
            default = Label("//:sxproto2textproto"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "_binaryproto2json": attr.label(
            default = Label("//:binaryproto2json"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)
