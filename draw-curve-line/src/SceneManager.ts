import * as BABYLON from "babylonjs";

export class SceneManager {
  static createDefaultScene(
    canvas: HTMLCanvasElement,
    cameraMode: number = BABYLON.Camera.PERSPECTIVE_CAMERA
  ): BABYLON.Scene {
    const engine = new BABYLON.Engine(canvas, true); // Generate the BABYLON 3D engine

    const scene: BABYLON.Scene = new BABYLON.Scene(engine);

    new BABYLON.DirectionalLight(
      "DirectionalLight",
      new BABYLON.Vector3(0, 0, 1),
      scene
    );

    // Parameters : name, position, scene
    // var camera = new BABYLON.UniversalCamera("UniversalCamera", new BABYLON.Vector3(0, 0, -10), scene);
    const camera = new BABYLON.ArcRotateCamera(
      "Camera",
      -Math.PI / 2,
      Math.PI / 2,
      10,
      new BABYLON.Vector3(0, 0, 0),
      scene
    );

    // Targets the camera to a particular position. In this case the scene origin
    camera.setTarget(BABYLON.Vector3.Zero());

    camera.mode = cameraMode;

    // Attach the camera to the canvas
    // camera.attachControl(canvas, true);
    camera.minZ = -1000;

    return scene;
  }
}
