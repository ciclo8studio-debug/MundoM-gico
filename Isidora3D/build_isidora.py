# Construye a Isidora sobre el cuerpo y el esqueleto del maniquí Quinn de Unreal.
# Uso: blender -b --python build_isidora.py
import bpy, bmesh, math
from mathutils import Vector, Matrix

DIR = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/"
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=DIR + "SKM_Quinn_Simple.fbx")

arm = [o for o in bpy.data.objects if o.type == 'ARMATURE'][0]
body = [o for o in bpy.data.objects if o.type == 'MESH'][0]
body.name = "Isidora"
bones = {b.name: arm.matrix_world @ b.head_local for b in arm.data.bones}
# trabajar en coordenadas de mundo: hornear la transformacion del cuerpo en la malla
mw = body.matrix_world.copy()
body.parent = None
body.data.transform(mw)
body.matrix_world = Matrix.Identity(4)
print("BODY_BOUNDS", [round(min(v.co[i] for v in body.data.vertices), 3) for i in range(3)], [round(max(v.co[i] for v in body.data.vertices), 3) for i in range(3)])

# ---------- materiales ----------
def mat(name, rgb, rough=0.7):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    p = m.node_tree.nodes.get("Principled BSDF")
    p.inputs["Base Color"].default_value = (*rgb, 1)
    p.inputs["Roughness"].default_value = rough
    m.diffuse_color = (*rgb, 1)
    return m

M = {
    'Piel': mat("M_Isi_Piel", (0.80, 0.60, 0.48), 0.55),
    'Chaqueta': mat("M_Isi_Chaqueta", (0.30, 0.05, 0.06), 0.85),
    'Falda': mat("M_Isi_Falda", (0.06, 0.06, 0.08), 0.8),
    'Medias': mat("M_Isi_Medias", (0.035, 0.035, 0.045), 0.6),
    'Botas': mat("M_Isi_Botas", (0.22, 0.12, 0.06), 0.5),
    'Pelo': mat("M_Isi_Pelo", (0.16, 0.09, 0.045), 0.6),
    'Mochila': mat("M_Isi_Mochila", (0.20, 0.22, 0.12), 0.9),
    'Ojos': mat("M_Isi_Ojos", (0.02, 0.015, 0.01), 0.2),
    'Blanco': mat("M_Isi_Blanco", (0.85, 0.84, 0.80), 0.3),
    'Iris': mat("M_Isi_Iris", (0.22, 0.12, 0.05), 0.2),
    'Labios': mat("M_Isi_Labios", (0.66, 0.36, 0.33), 0.5),
}
body.data.materials.clear()
for k in M:
    body.data.materials.append(M[k])
MI = {k: i for i, k in enumerate(M)}

def gweights(obj):
    names = {g.index: g.name for g in obj.vertex_groups}
    out = []
    for v in obj.data.vertices:
        out.append({names[g.group]: g.weight for g in v.groups})
    return out

def wsum(w, prefixes):
    return sum(val for n, val in w.items() if any(n.startswith(p) for p in prefixes))

# ---------- cabeza mas grande (proporcion de nina) ----------
W = gweights(body)
hc = bones['head'] + Vector((0, -0.01, 0.07))
for v, w in zip(body.data.vertices, W):
    hw = wsum(w, ('head',)) + 0.5 * wsum(w, ('neck_02',))
    if hw > 0:
        t = min(1, hw)
        d = v.co - hc
        v.co = hc + Vector((d.x * (1 + 0.24 * t), d.y * (1 + 0.16 * t), d.z * (1 + 0.06 * t)))
        rel = v.co - hc
        # menton mas corto y redondo
        if rel.z < -0.06:
            rel.z = -0.06 + (rel.z + 0.06) * (1 - 0.32 * t)
            rel.x *= 1 - 0.06 * t
        # mejillas un poco mas llenas
        if rel.y < -0.05 and 0.025 < abs(rel.x) < 0.07 and -0.06 < rel.z < 0.0:
            fall = (1 - abs(abs(rel.x) - 0.047) / 0.023) * (1 - abs(rel.z + 0.03) / 0.03)
            rel.y -= 0.005 * max(0, fall) * t
        v.co = hc + rel

# piel y medias en el cuerpo base
bm = bmesh.new(); bm.from_mesh(body.data)
for f in bm.faces:
    z = f.calc_center_median().z
    f.material_index = MI['Medias'] if 0.08 < z < 0.74 else MI['Piel']
bm.to_mesh(body.data); bm.free()

# ---------- prendas a partir del cuerpo ----------
def shell(name, keep, offset, matkey, smooth=0):
    o = body.copy(); o.data = body.data.copy(); o.name = name
    bpy.context.collection.objects.link(o)
    Wo = gweights(o)
    bm = bmesh.new(); bm.from_mesh(o.data); bm.verts.ensure_lookup_table()
    dele = [v for v in bm.verts if not keep(v.co, Wo[v.index])]
    bmesh.ops.delete(bm, geom=dele, context='VERTS')
    bm.normal_update()
    for v in bm.verts:
        v.co += v.normal * offset
    if smooth:
        inner = [v for v in bm.verts if not v.is_boundary]
        for _ in range(smooth):
            bmesh.ops.smooth_vert(bm, verts=inner, factor=0.5, use_axis_x=True, use_axis_y=True, use_axis_z=True)
        bm.normal_update()
        for v in inner:
            v.co += v.normal * offset * 0.4
    for f in bm.faces:
        f.material_index = MI[matkey]
    bm.to_mesh(o.data); bm.free()
    return o

parts = []
# chaqueta: torso y brazos hasta la muneca, cuello bajo
parts.append(shell("Chaqueta", lambda c, w: wsum(w, ('spine', 'clavicle', 'upperarm', 'lowerarm', 'pelvis')) > 0.35
                   and wsum(w, ('hand',)) < 0.3 and 0.86 < c.z < 1.56 and wsum(w, ('head', 'neck_02')) < 0.2, 0.016, 'Chaqueta', smooth=25))
# botas
parts.append(shell("Botas", lambda c, w: c.z < 0.24, 0.012, 'Botas'))
# casco de pelo: parte alta y trasera de la cabeza
def pelo_keep(c, w):
    if wsum(w, ('head',)) < 0.6: return False
    rel = c - hc
    front = rel.y < -0.045 and rel.z < 0.07 and abs(rel.x) < 0.064
    return not front and rel.z > -0.075
parts.append(shell("Pelo", pelo_keep, 0.014, 'Pelo', smooth=8))

# ---------- piezas modeladas ----------
def add_obj(name, bm, matkey, double=False):
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    if double:
        # caras por ambos lados (falda/capucha abiertas)
        ret = bmesh.ops.duplicate(bm, geom=list(bm.faces))
        nf = [g for g in ret['geom'] if isinstance(g, bmesh.types.BMFace)]
        bmesh.ops.reverse_faces(bm, faces=nf)
        bm.normal_update()
        for v in {v for f in nf for v in f.verts}:
            v.co -= v.normal * 0.004
    me = bpy.data.meshes.new(name); bm.to_mesh(me); bm.free()
    o = bpy.data.objects.new(name, me); bpy.context.collection.objects.link(o)
    o.data.materials.append(M[matkey])
    for p in o.data.polygons: p.use_smooth = True
    return o

def falda():
    bm = bmesh.new()
    rings, seg = 7, 48
    top, bot = bones['pelvis'].z + 0.02, 0.66
    rows = []
    for i in range(rings):
        t = i / (rings - 1)
        z = top + (bot - top) * t
        r0 = 0.165 + 0.11 * t
        row = []
        for j in range(seg):
            a = 2 * math.pi * j / seg
            pleat = 1 + 0.045 * t * math.cos(a * 16)
            row.append(bm.verts.new((math.cos(a) * r0 * pleat * 1.05, math.sin(a) * r0 * pleat * 0.86 - 0.02, z)))
        rows.append(row)
    for i in range(rings - 1):
        for j in range(seg):
            bm.faces.new((rows[i][j], rows[i][(j + 1) % seg], rows[i + 1][(j + 1) % seg], rows[i + 1][j]))
    return add_obj("Falda", bm, 'Falda', double=True)

def coleta():
    bm = bmesh.new()
    base = hc + Vector((0, 0.125, 0.03))
    pts = [base + Vector((0, 0.05 * math.sin(t * 2.2) + 0.02 * t, -0.28 * t)) for t in [i / 9 for i in range(10)]]
    seg = 10; rows = []
    for k, p in enumerate(pts):
        t = k / (len(pts) - 1)
        r = 0.032 * (1 - 0.75 * t) + 0.006
        rows.append([bm.verts.new(p + Vector((math.cos(2 * math.pi * j / seg) * r, math.sin(2 * math.pi * j / seg) * r * 0.8, 0))) for j in range(seg)])
    for i in range(len(rows) - 1):
        for j in range(seg):
            bm.faces.new((rows[i][j], rows[i][(j + 1) % seg], rows[i + 1][(j + 1) % seg], rows[i + 1][j]))
    bm.faces.new(rows[-1][::-1])
    # liga del pelo
    return add_obj("Coleta", bm, 'Pelo')

def caja(name, center, size, matkey, bevel=0.03):
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=1)
    for v in bm.verts:
        v.co = Vector((v.co.x * size[0], v.co.y * size[1], v.co.z * size[2])) + center
    bmesh.ops.bevel(bm, geom=list(bm.edges), offset=bevel, segments=3, affect='EDGES')
    return add_obj(name, bm, matkey)

def esfera(name, center, r, matkey, sc=(1, 1, 1)):
    bm = bmesh.new()
    bmesh.ops.create_uvsphere(bm, u_segments=12, v_segments=8, radius=r)
    for v in bm.verts:
        v.co = Vector((v.co.x * sc[0], v.co.y * sc[1], v.co.z * sc[2])) + center
    return add_obj(name, bm, matkey)

def tubo(name, pts, r, matkey, seg=8):
    bm = bmesh.new(); rows = []
    for i, p in enumerate(pts):
        d = (pts[min(i + 1, len(pts) - 1)] - pts[max(i - 1, 0)]).normalized()
        a = d.orthogonal().normalized(); b = d.cross(a)
        rows.append([bm.verts.new(p + (a * math.cos(2 * math.pi * j / seg) + b * math.sin(2 * math.pi * j / seg)) * r) for j in range(seg)])
    for i in range(len(rows) - 1):
        for j in range(seg):
            bm.faces.new((rows[i][j], rows[i][(j + 1) % seg], rows[i + 1][(j + 1) % seg], rows[i + 1][j]))
    return add_obj(name, bm, matkey)

parts.append(falda())
parts.append(coleta())
parts.append(esfera("Liga", hc + Vector((0, 0.122, 0.03)), 0.028, 'Chaqueta', (1, 0.7, 0.5)))
# capucha caida en la espalda
parts.append(esfera("Capucha", bones['neck_01'] + Vector((0, 0.085, -0.015)), 0.11, 'Chaqueta', (1.05, 0.55, 0.55)))
# mochila y correas
sp = bones['spine_04']
parts.append(caja("Mochila", sp + Vector((0, 0.19, 0.02)), (0.27, 0.12, 0.33), 'Mochila', 0.035))
parts.append(caja("Bolsillo", sp + Vector((0, 0.26, -0.07)), (0.20, 0.04, 0.12), 'Mochila', 0.02))
for sx in (-1, 1):
    pts = [sp + Vector((0.08 * sx, 0.14, 0.16)), bones['clavicle_l' if sx > 0 else 'clavicle_r'] + Vector((0.07 * sx, 0.05, 0.05)),
           bones['clavicle_l' if sx > 0 else 'clavicle_r'] + Vector((0.08 * sx, -0.09, -0.04)), sp + Vector((0.10 * sx, -0.11, -0.12))]
    parts.append(tubo("Correa", pts, 0.012, 'Mochila'))
# ---------- cara detallada, pegada a la superficie con rayos ----------
from mathutils.bvhtree import BVHTree
_bm = bmesh.new(); _bm.from_mesh(body.data); TREE = BVHTree.FromBMesh(_bm)
def surf(x, z, frm=None, d=Vector((0, 1, 0))):
    o = frm if frm is not None else Vector((x, hc.y - 0.6, z))
    hit = TREE.ray_cast(o, d, 2.0)
    return (hit[0], hit[1]) if hit[0] is not None else (Vector((x, hc.y - 0.11, z)), Vector((0, -1, 0)))
def sph(name, c, r, matkey, sc=(1, 1, 1), seg=(16, 10)):
    bm = bmesh.new()
    bmesh.ops.create_uvsphere(bm, u_segments=seg[0], v_segments=seg[1], radius=r)
    for v in bm.verts:
        v.co = Vector((v.co.x * sc[0], v.co.y * sc[1], v.co.z * sc[2])) + c
    return add_obj(name, bm, matkey)
def arco(name, xs, zf, off, r, matkey):
    pts = []
    for x in xs:
        p, n = surf(x, zf(x))
        pts.append(p + n * off)
    return tubo(name, pts, r, matkey, seg=6)
ez = hc.z + 0.012
for sx in (-1, 1):
    ex_ = 0.036 * sx
    p, n = surf(ex_, ez)
    parts.append(sph("OjoBlanco", p + n * 0.0015, 0.0195, 'Blanco', (1, 0.35, 0.66)))
    parts.append(sph("Iris", p + n * 0.0078 + Vector((0, 0, -0.0005)), 0.0095, 'Iris', (1, 0.35, 1)))
    parts.append(sph("Pupila", p + n * 0.0102, 0.0046, 'Ojos', (1, 0.35, 1)))
    parts.append(sph("Brillo", p + n * 0.0118 + Vector((0.0028 * sx, 0, 0.0038)), 0.0016, 'Blanco', (1, 0.5, 1), (8, 6)))
    xs = [ex_ + sx * t for t in (-0.019, -0.012, -0.004, 0.004, 0.012, 0.019)]
    parts.append(arco("Parpado", xs, lambda x, e=ex_: ez + 0.0085 + 0.004 * math.cos((x - e) / 0.019 * 1.4), 0.0035, 0.0017, 'Ojos'))
    parts.append(arco("Pestanas", [x + sx * 0.002 for x in xs[3:]] + [ex_ + sx * 0.023], lambda x, e=ex_: ez + 0.009 + 0.006 * max(0, (x - e) * sx) / 0.023, 0.004, 0.0012, 'Ojos'))
    parts.append(arco("Ceja", [ex_ + sx * t for t in (-0.020, -0.008, 0.006, 0.020)], lambda x, e=ex_: ez + 0.030 + 0.006 * math.cos((x - e) / 0.02 * 1.2) - 0.002 * (x - e) * sx / 0.02, 0.003, 0.0028, 'Pelo'))
    # orejas
    q, m = surf(0, 0, frm=Vector((sx * 0.4, hc.y + 0.005, ez - 0.02)), d=Vector((-sx, 0, 0)))
    parts.append(sph("Oreja", q + m * 0.004, 0.009, 'Piel', (0.6, 1.0, 1.5)))
# nariz: puente y punta
pn, nn = surf(0, ez - 0.032)
parts.append(sph("Nariz", pn + nn * 0.002, 0.0115, 'Piel', (1.0, 0.75, 0.85)))
for sx in (-1, 1):
    pa, na = surf(0.0055 * sx, ez - 0.039)
    parts.append(sph("Fosa", pa + na * 0.004 + Vector((0, 0, -0.002)), 0.0022, 'Ojos', (1, 0.6, 0.6), (8, 6)))
# boca
pm, nm = surf(0, ez - 0.058)
parts.append(sph("LabioSup", pm + nm * 0.0025 + Vector((0, 0, 0.003)), 0.016, 'Labios', (1, 0.28, 0.22)))
parts.append(sph("LabioInf", pm + nm * 0.0028 + Vector((0, 0, -0.004)), 0.0135, 'Labios', (1, 0.32, 0.3)))
parts.append(arco("LineaBoca", [-0.014, -0.007, 0, 0.007, 0.014], lambda x: ez - 0.0575 - 0.0015 * (x / 0.014) ** 2, 0.004, 0.0011, 'Ojos'))
# menton y mejillas suaves

# flequillo
parts.append(esfera("Flequillo", hc + Vector((0, -0.088, 0.08)), 0.085, 'Pelo', (1.02, 0.42, 0.30)))
# mechones a los lados de la cara
for sx in (-1, 1):
    pts = [hc + Vector((0.060 * sx, -0.070, 0.060 - 0.012 * i)) + Vector((0.004 * sx * math.sin(i * 0.6), 0.003 * i, 0)) for i in range(9)]
    bmm = bmesh.new(); rows = []
    for i, p_ in enumerate(pts):
        r = 0.013 * (1 - 0.6 * i / 8) + 0.003
        rows.append([bmm.verts.new(p_ + Vector((math.cos(2 * math.pi * j / 8) * r * 0.7, math.sin(2 * math.pi * j / 8) * r, 0))) for j in range(8)])
    for i in range(len(rows) - 1):
        for j in range(8):
            bmm.faces.new((rows[i][j], rows[i][(j + 1) % 8], rows[i + 1][(j + 1) % 8], rows[i + 1][j]))
    bmm.faces.new(rows[-1][::-1])
    parts.append(add_obj("Mechon", bmm, 'Pelo'))

# ---------- pesos: copiar del cuerpo ----------
bpy.ops.object.select_all(action='DESELECT')
for o in parts:
    for g in body.vertex_groups:
        if g.name not in o.vertex_groups:
            o.vertex_groups.new(name=g.name)
    dt = o.modifiers.new("dt", 'DATA_TRANSFER')
    dt.object = body
    dt.use_vert_data = True
    dt.data_types_verts = {'VGROUP_WEIGHTS'}
    dt.vert_mapping = 'POLYINTERP_NEAREST'
    dt.layers_vgroup_select_src = 'ALL'
    dt.layers_vgroup_select_dst = 'NAME'
    bpy.context.view_layer.objects.active = o
    bpy.ops.object.datalayout_transfer(modifier="dt")
    bpy.ops.object.modifier_apply(modifier="dt")

# la falda no debe estirarse con los muslos: mas peso a la pelvis
fal = [o for o in parts if o.name.startswith("Falda")][0]
gp = fal.vertex_groups['pelvis']
for v in fal.data.vertices:
    t = (bones['pelvis'].z + 0.02 - v.co.z) / 0.35
    for g in v.groups:
        g.weight *= 0.5 * t
    gp.add([v.index], 1 - 0.5 * t, 'ADD')

# ---------- unir todo ----------
bpy.ops.object.select_all(action='DESELECT')
for o in parts + [body]:
    o.select_set(True)
bpy.context.view_layer.objects.active = body
bpy.ops.object.join()
for p in body.data.polygons:
    p.use_smooth = True
bpy.ops.object.select_all(action='DESELECT')
body.select_set(True)
bpy.ops.object.vertex_group_normalize_all(lock_active=False)

# ---------- exportar ----------
body.parent = arm
body.matrix_parent_inverse = arm.matrix_world.inverted()
arm.select_set(True)
body.select_set(True)
bpy.ops.export_scene.fbx(filepath=DIR + "SKM_Isidora.fbx", use_selection=True, add_leaf_bones=False,
                         bake_anim=False, mesh_smooth_type='FACE', object_types={'ARMATURE', 'MESH'})
bpy.ops.wm.save_as_mainfile(filepath=DIR + "Isidora.blend")

# ---------- vista previa ----------
scn = bpy.context.scene
scn.render.engine = 'BLENDER_EEVEE' if 'BLENDER_EEVEE' in [e.identifier for e in bpy.types.RenderSettings.bl_rna.properties['engine'].enum_items] else 'BLENDER_WORKBENCH'
scn.render.resolution_x, scn.render.resolution_y = 800, 900
cam = bpy.data.objects.new("Cam", bpy.data.cameras.new("Cam")); scn.collection.objects.link(cam); scn.camera = cam
for name, loc, tgt in (("front", (0.9, -3.0, 1.3), (0, 0, 0.95)), ("back", (-0.9, 3.0, 1.3), (0, 0, 0.95)), ("face", (0.12, -0.62, 1.72), (0, 0, hc.z))):
    cam.location = loc
    d = Vector(tgt) - cam.location
    cam.rotation_euler = d.to_track_quat('-Z', 'Y').to_euler()
    lt = bpy.data.objects.new("L" + name, bpy.data.lights.new("L" + name, 'SUN')); scn.collection.objects.link(lt)
    lt.rotation_euler = cam.rotation_euler; lt.data.energy = 3
    scn.render.filepath = DIR + "preview_%s.png" % name
    bpy.ops.render.render(write_still=True)
print("BUILD_OK")
