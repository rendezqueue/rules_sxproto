
def _descriptor_set_depset(ctx):
  transitive_descriptor_sets = [
      dep[ProtoInfo].transitive_descriptor_sets
      for dep in ctx.attr.proto_deps
  ]
  return depset(ctx.files.proto_deps,
                transitive=transitive_descriptor_sets)


def _binaryproto_file(ctx):
  binaryproto_file = None
  if ctx.outputs.out_binaryproto:
    binaryproto_file = ctx.outputs.out_binaryproto
  else:
    binaryproto_file = ctx.actions.declare_file(ctx.label.name + ".binaryproto")
  return binaryproto_file


def _textproto_data_impl(ctx, textproto_file=None, outfiles=None):
  if not textproto_file:
    textproto_file = ctx.file.src
  if not outfiles:
    outfiles = []

  descriptor_set_depset = _descriptor_set_depset(ctx)

  # Compile to binaryproto.
  binaryproto_file = _binaryproto_file(ctx)
  outfiles = [binaryproto_file] + outfiles  # Prepend.
  args = ctx.actions.args()
  args.add(textproto_file)
  args.add(binaryproto_file)
  args.add(ctx.attr.proto_message)
  args.add_all(descriptor_set_depset)
  ctx.actions.run(
      executable = ctx.executable._textproto2binaryproto,
      arguments = [args],
      inputs = depset([textproto_file], transitive=[descriptor_set_depset]),
      outputs = [binaryproto_file],
   )

  if ctx.outputs.out_json:
    # Translate to JSON after the fact.
    json_file = ctx.outputs.out_json
    outfiles.append(json_file)
    args = ctx.actions.args()
    args.add(binaryproto_file)
    args.add(json_file)
    args.add(ctx.attr.proto_message)
    args.add_all(descriptor_set_depset)
    ctx.actions.run(
        executable = ctx.executable._binaryproto2json,
        arguments = [args],
        inputs = depset([binaryproto_file], transitive=[descriptor_set_depset]),
        outputs = [json_file],
     )

  if ctx.outputs.out_json_camelcase:
    # Translate to JSON after the fact.
    json_camelcase_file = ctx.outputs.out_json_camelcase
    outfiles.append(json_camelcase_file)
    args = ctx.actions.args()
    args.add(binaryproto_file)
    args.add(json_camelcase_file)
    args.add(ctx.attr.proto_message)
    args.add_all(descriptor_set_depset)
    ctx.actions.run(
        executable = ctx.executable._binaryproto2json_camelcase,
        arguments = [args],
        inputs = depset([binaryproto_file], transitive=[descriptor_set_depset]),
        outputs = [json_camelcase_file],
     )

  return DefaultInfo(
      files = depset([binaryproto_file]),
      runfiles = ctx.runfiles(files = outfiles),
  )


def _sxproto_data_impl(ctx):
  sxproto_file = ctx.file.src

  textproto_file = None
  outfiles = []
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
  return _textproto_data_impl(
      ctx, textproto_file=textproto_file, outfiles=outfiles)


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
        "out_json_camelcase": attr.output(
            mandatory = False,
            doc = "The .json file to write with camelCase fields.",
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
        "_textproto2binaryproto": attr.label(
            default = Label("//:textproto2binaryproto"),
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
        "_binaryproto2json_camelcase": attr.label(
            default = Label("//:binaryproto2json_camelcase"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)

textproto_data = rule(
    implementation = _textproto_data_impl,
    attrs = {
        "src": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "The .textproto file to compile.",
        ),
        "out_binaryproto": attr.output(
            mandatory = False,
            doc = "The .binaryproto file to write.",
        ),
        "out_json": attr.output(
            mandatory = False,
            doc = "The .json file to write.",
        ),
        "out_json_camelcase": attr.output(
            mandatory = False,
            doc = "The .json file to write with camelCase fields.",
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
        "_textproto2binaryproto": attr.label(
            default = Label("//:textproto2binaryproto"),
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
        "_binaryproto2json_camelcase": attr.label(
            default = Label("//:binaryproto2json_camelcase"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)
