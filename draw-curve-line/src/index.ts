import { SceneManager } from "./SceneManager";
import * as BABYLON from "babylonjs";

class Point {
  vector: BABYLON.Vector3;
  time: number = 0;
  constructor(vector: BABYLON.Vector3) {
    this.vector = vector;
    this.time = Date.now();
  }
}

let lines: BABYLON.Mesh[] = [];
let engine: BABYLON.Engine;

const line2D = function (name: any, options: any, scene: any): BABYLON.Mesh {
  //Arrays for vertex positions and indices
  var positions: any = [];
  var indices: any = [];
  var normals: any = [];
  var name = name;

  var width = options.width / 2 || 0.5;
  var path = options.path;
  var standardUV: any;
  var direction: any;
  var lineNormal: any;
  var closed = options.closed || false;
  if (options.standardUV === undefined) {
    standardUV = true;
  } else {
    standardUV = options.standardUV;
  }

  //   var interiorIndex;

  //Arrays to hold wall corner data
  //   var innerBaseCorners = [];
  //   var outerBaseCorners = [];

  var outerData = [];
  var innerData = [];
  var angle = 0;

  var nbPoints = path.length;
  var line = BABYLON.Vector3.Zero();
  var nextLine = BABYLON.Vector3.Zero();
  path[1].subtractToRef(path[0], line);

  if (nbPoints > 2 && closed) {
    path[2].subtractToRef(path[1], nextLine);
    for (var p = 0; p < nbPoints; p++) {
      angle =
        Math.PI -
        Math.acos(
          BABYLON.Vector3.Dot(line, nextLine) /
            (line.length() * nextLine.length())
        );
      direction = BABYLON.Vector3.Cross(line, nextLine).normalize().z;
      lineNormal = new BABYLON.Vector3(line.y, -1 * line.x, 0).normalize();
      line.normalize();
      innerData[(p + 1) % nbPoints] = path[(p + 1) % nbPoints]
        .subtract(lineNormal.scale(width))
        .subtract(line.scale((direction * width) / Math.tan(angle / 2)));
      outerData[(p + 1) % nbPoints] = path[(p + 1) % nbPoints]
        .add(lineNormal.scale(width))
        .add(line.scale((direction * width) / Math.tan(angle / 2)));
      line = nextLine.clone();
      path[(p + 3) % nbPoints].subtractToRef(
        path[(p + 2) % nbPoints],
        nextLine
      );
    }
  } else {
    lineNormal = new BABYLON.Vector3(line.y, -1 * line.x, 0).normalize();
    line.normalize();
    innerData[0] = path[0].subtract(lineNormal.scale(width));
    outerData[0] = path[0].add(lineNormal.scale(width));

    for (var p = 0; p < nbPoints - 2; p++) {
      path[p + 2].subtractToRef(path[p + 1], nextLine);
      angle =
        Math.PI -
        Math.acos(
          BABYLON.Vector3.Dot(line, nextLine) /
            (line.length() * nextLine.length())
        );
      direction = BABYLON.Vector3.Cross(line, nextLine).normalize().z;
      lineNormal = new BABYLON.Vector3(line.y, -1 * line.x, 0).normalize();
      line.normalize();
      innerData[p + 1] = path[p + 1]
        .subtract(lineNormal.scale(width))
        .subtract(line.scale((direction * width) / Math.tan(angle / 2)));
      outerData[p + 1] = path[p + 1]
        .add(lineNormal.scale(width))
        .add(line.scale((direction * width) / Math.tan(angle / 2)));
      line = nextLine.clone();
    }
    if (nbPoints > 2) {
      path[nbPoints - 1].subtractToRef(path[nbPoints - 2], line);
      lineNormal = new BABYLON.Vector3(line.y, -1 * line.x, 0).normalize();
      line.normalize();
      innerData[nbPoints - 1] = path[nbPoints - 1].subtract(
        lineNormal.scale(width)
      );
      outerData[nbPoints - 1] = path[nbPoints - 1].add(lineNormal.scale(width));
    } else {
      innerData[1] = path[1].subtract(lineNormal.scale(width));
      outerData[1] = path[1].add(lineNormal.scale(width));
    }
  }

  var maxX = Number.MIN_VALUE;
  var minX = Number.MAX_VALUE;
  var maxY = Number.MIN_VALUE;
  var minY = Number.MAX_VALUE;

  for (var p = 0; p < nbPoints; p++) {
    positions.push(innerData[p].x, innerData[p].y, innerData[p].z);
    maxX = Math.max(innerData[p].x, maxX);
    minX = Math.min(innerData[p].x, minX);
    maxY = Math.max(innerData[p].y, maxY);
    minY = Math.min(innerData[p].y, minY);
  }

  for (var p = 0; p < nbPoints; p++) {
    positions.push(outerData[p].x, outerData[p].y, outerData[p].z);
    maxX = Math.max(innerData[p].x, maxX);
    minX = Math.min(innerData[p].x, minX);
    maxY = Math.max(innerData[p].y, maxY);
    minY = Math.min(innerData[p].y, minY);
  }

  for (var i = 0; i < nbPoints - 1; i++) {
    indices.push(i, i + 1, nbPoints + i + 1);
    indices.push(i, nbPoints + i + 1, nbPoints + i);
  }

  if (nbPoints > 2 && closed) {
    indices.push(nbPoints - 1, 0, nbPoints);
    indices.push(nbPoints - 1, nbPoints, 2 * nbPoints - 1);
  }

  var normals: any = [];
  var uvs = [];

  if (standardUV) {
    for (var p = 0; p < positions.length; p += 3) {
      uvs.push(
        (positions[p] - minX) / (maxX - minX),
        (positions[p + 1] - minY) / (maxY - minY)
      );
    }
  } else {
    var flip = 0;
    var p1 = 0;
    var p2 = 0;
    var p3 = 0;
    var v0 = innerData[0];
    var v1 = innerData[1].subtract(v0);
    var v2 = outerData[0].subtract(v0);
    var v3 = outerData[1].subtract(v0);
    var axis = v1.clone();
    axis.normalize();

    p1 = BABYLON.Vector3.Dot(axis, v1);
    p2 = BABYLON.Vector3.Dot(axis, v2);
    p3 = BABYLON.Vector3.Dot(axis, v3);
    var minX = Math.min(0, p1, p2, p3);
    var maxX = Math.max(0, p1, p2, p3);

    uvs[2 * indices[0]] = -minX / (maxX - minX);
    uvs[2 * indices[0] + 1] = 1;
    uvs[2 * indices[5]] = (p2 - minX) / (maxX - minX);
    uvs[2 * indices[5] + 1] = 0;

    uvs[2 * indices[1]] = (p1 - minX) / (maxX - minX);
    uvs[2 * indices[1] + 1] = 1;
    uvs[2 * indices[4]] = (p3 - minX) / (maxX - minX);
    uvs[2 * indices[4] + 1] = 0;

    for (var i = 6; i < indices.length; i += 6) {
      flip = (flip + 1) % 2;
      v0 = innerData[0];
      v1 = innerData[1].subtract(v0);
      v2 = outerData[0].subtract(v0);
      v3 = outerData[1].subtract(v0);
      axis = v1.clone();
      axis.normalize();

      p1 = BABYLON.Vector3.Dot(axis, v1);
      p2 = BABYLON.Vector3.Dot(axis, v2);
      p3 = BABYLON.Vector3.Dot(axis, v3);
      var minX = Math.min(0, p1, p2, p3);
      var maxX = Math.max(0, p1, p2, p3);

      uvs[2 * indices[i + 1]] =
        flip + (Math.cos(flip * Math.PI) * (p1 - minX)) / (maxX - minX);
      uvs[2 * indices[i + 1] + 1] = 1;
      uvs[2 * indices[i + 4]] =
        flip + (Math.cos(flip * Math.PI) * (p3 - minX)) / (maxX - minX);
      uvs[2 * indices[i + 4] + 1] = 0;
    }
  }

  BABYLON.VertexData.ComputeNormals(positions, indices, normals);
  BABYLON.VertexData._ComputeSides(
    BABYLON.Mesh.DOUBLESIDE,
    positions,
    indices,
    normals,
    uvs
  );
  // console.log(uvs);
  //Create a custom mesh
  var customMesh = new BABYLON.Mesh("custom", scene);

  //Create a vertexData object
  var vertexData = new BABYLON.VertexData();

  //Assign positions and indices to vertexData
  vertexData.positions = positions;
  vertexData.indices = indices;
  vertexData.normals = normals;
  vertexData.uvs = uvs;

  //Apply vertexData to custom mesh
  vertexData.applyToMesh(customMesh);

  return customMesh;
};

let points: Point[] = [];

const canvas = document.getElementById("renderCanvas");
if (canvas instanceof HTMLCanvasElement) {
  const scene = SceneManager.createDefaultScene(
    canvas,
    BABYLON.Camera.ORTHOGRAPHIC_CAMERA
  );

  engine = scene.getEngine();

  displayCoordinate();

  const deviceSourceManager = new BABYLON.DeviceSourceManager(
    scene.getEngine()
  );

  deviceSourceManager.onDeviceConnectedObservable.add((deviceSource) => {
    if (deviceSource.deviceType === BABYLON.DeviceType.Mouse) {
      deviceSource.onInputChangedObservable.add((eventData) => {

        // screen coordinate to scene coordinate
        let p = BABYLON.Vector3.Unproject(
          new BABYLON.Vector3(eventData.x, eventData.y, 0),
          engine.getRenderWidth(),
          engine.getRenderHeight(),
          BABYLON.Matrix.Identity(),
          scene.getViewMatrix(),
          scene.getProjectionMatrix()
        );

        points.push(new Point(p));
      });
    }

    engine.runRenderLoop(() => {
      clearLines();
      updatePoints();
      drawLines(points, scene);
      scene.render();
      displayFPS();
    });
  });
}

function updatePoints(): void {
  let p: Point[] = [];
  let time: number = Date.now();
  for (let i = 0; i < points.length; i++) {
    if (time - points[i].time < 200) {
      p.push(points[i]);
    }
  }

  points = p;
}

function clearLines(): void {
  lines.forEach((value: BABYLON.Mesh) => {
    value.dispose();
  });
  lines.length = 0;
}

function drawLines(points: Point[], scene: BABYLON.Scene): void {
  if (points.length <= 1) {
    return;
  }

  let width = 10;
  const step = width / (points.length - 1);
  for (let i = points.length - 1; i >= 1; i--) {
    let line = line2D(
      "line" + i,
      { path: [points[i].vector, points[i - 1].vector], width: width },
      scene
    );
    lines.push(line);
    width -= step;
    if (width < 1) {
      width = 1;
    }
  }
}

function displayFPS(): void {
  const element = document.getElementById("fps");
  if (element !== null) {
    element.innerHTML = "FPS: " + engine.getFps().toFixed();
  }
}

function displayCoordinate(): void {
  BABYLON.MeshBuilder.CreateLines("x", {
    points: [new BABYLON.Vector3(0, 0, 0), new BABYLON.Vector3(100, 0, 0)],
    colors: [new BABYLON.Color4(1, 0, 0, 1), new BABYLON.Color4(1, 0, 0, 1)],
  });
  BABYLON.MeshBuilder.CreateLines("y", {
    points: [new BABYLON.Vector3(0, 0, 0), new BABYLON.Vector3(0, 100, 0)],
    colors: [new BABYLON.Color4(0, 1, 0, 1), new BABYLON.Color4(0, 1, 0, 1)],
  });
  BABYLON.MeshBuilder.CreateLines("z", {
    points: [new BABYLON.Vector3(0, 0, 0), new BABYLON.Vector3(0, 0, 100)],
    colors: [new BABYLON.Color4(0, 0, 1, 1), new BABYLON.Color4(0, 0, 1, 1)],
  });
}

export {};
