import { createRoot } from "react-dom/client";
import { FaArrowTurnDown, FaBook, FaBox, FaCube, FaGithub, FaGlobe } from "react-icons/fa6";
import hljs from "highlight.js/lib/core";
import "highlight.js/styles/atom-one-dark.css";

hljs.registerLanguage("cpp", require("highlight.js/lib/languages/cpp"));

import menuImg from "./menu.png";
import ecsImg from "./ecs.png";
import "./index.css";

function Link(props: { href: string, children: React.ReactNode }) {
  return (
    <a href={props.href} className="text-green-500 hover:bg-green-600/30 transition duration-200 underline rounded-md px-px">
      {props.children}
    </a>
  )
}

function Ref(props: { r: number }) {
  return (
    <sup id={`refs.${props.r}`}>
      <Link href={`#footnotes.${props.r}`}>
        [{props.r}]
      </Link>
    </sup>
  )
}

function FootNote(props: { r: number, children?: React.ReactNode }) {
  return (
    <span id={`footnotes.${props.r}`}>
      {props.children}
      <Link href={`#refs.${props.r}`}>
        <FaArrowTurnDown className="pl-2 inline"/>
      </Link>
    </span>
  )
}

function App() {
  return (
    <div className="flex flex-col text-black dark:text-white bg-white dark:bg-black items-center px-4 py-8">
      <div className="size-full flex flex-col max-w-6xl">
        <h1 className="text-4xl">Qun</h1>
        <hr className="my-2 w-full border-black dark:border-white" />

        <p>
          <Link href="https://github.com/DvvCz/qun">Qun</Link> is a project I created for Professor Fahim Khan's CSC 471 class for Spring 2025 at Cal Poly.
        </p>

        <p>
          The assignment was to create a renderer using OpenGL, and to create a scene with a few requirements.
        </p>

        <h2 className="text-3xl mt-8">Draft</h2>
        <hr className="my-2 w-1/2 border-black dark:border-white" />

        <div className="flex flex-col md:flex-row gap-8 md:gap-2">
          <div className="md:w-1/2 flex flex-col gap-2">
            <p>
              When thinking of interesting ideas, I remembered a game I played as a kid, <Link href="https://en.wikipedia.org/wiki/Need_for_Speed:_Carbon">Need For Speed Carbon.</Link>
            </p>

            <p>
              The concept was to mostly go for a good looking scene with a camera that would move around into different perspectives around the car.
            </p>

            <p>
              After consulting with the professor, the direction of the project moved to a more dynamic scene, controlling the car instead.
            </p>
          </div>

          <div className="md:w-1/2 h-56">
            <img className="size-full object-contain" src={menuImg} />
          </div>
        </div>

        <h2 className="text-3xl mt-6">Architecture</h2>
        <hr className="my-2 w-1/2 border-black dark:border-white" />

        <div>
          <p>
            Entering the project, I knew I was going to need some large changes, the first being to have support for assets with multiple textures inside of them.
          </p>

          <p className="mt-2">
            Additionally, I've always been interested in getting into graphics programming, so I opted to start entirely from scratch, no base code.
          </p>
          <p>
            Therefore, I created a new project using C++23 and <i>modern</i> OpenGL <Ref r={1} />
          </p>

          <p className="mt-2">
            I have some experience working with game engines like <Link href="https://bevy.org">Bevy</Link>, alongside other work, which introduced me to the concept of the <Link href="https://taintedcoders.com/bevy/ecs">ECS</Link>. I wanted to implement the concepts into my new renderer. At this point, I decided I was making a mini game engine.
          </p>

          <div className="mt-6">
            <h3 className="text-2xl text-yellow-500 dark:text-yellow-300">- Assets</h3>
            <hr className="my-2 w-1/2 border-black dark:border-white" />

            <div className="flex flex-col gap-2">
              <p>
                The biggest problem I had with the given code was .obj assets. They're hardly used in the industry, which leads to assets not being ideal and oftentimes even broken.
              </p>

              <p>
                I was well aware of the <Link href="https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html">GLTF</Link> file format as an alternative, which was made by the Khronos group (the same group that made OpenGL), which is much more commonly used, and has a convenient binary format so that assets can be single files.
              </p>

              <p>
                I leveraged the <Link href="https://github.com/spnda/fastgltf">fastgltf</Link> library and created a GLTF asset loader, which did a bunch of things like converting the format from using PBR <Ref r={2} /> assets to phong, which my renderer still uses, although later on PBR might be the default.
              </p>
            </div>
          </div>

          <div className="mt-6">
            <h3 className="text-2xl text-yellow-500 dark:text-yellow-300">- Entities</h3>
            <hr className="my-2 w-1/2 border-black dark:border-white" />

            <div className="flex flex-col md:flex-row gap-4">
              <div className="md:w-1/2 flex flex-col gap-2">
                <p>To incorporate dynamic entities, I chose ECS, to have everything be an entity with different components.</p>

                <p>
                  I leveraged the <Link href="https://github.com/skypjack/entt">EnTT</Link> library, which is very widely used, even used by <Link href="https://www.minecraft.net/en-us/attribution">Minecraft (Bedrock)!</Link>
                </p>

                <p>
                  It's a relatively simple concept - all <strong>E</strong>ntities are just IDs. You give the IDs <strong>C</strong>omponents, which are just data. You can then create <strong>S</strong>ystems that operate on the components of entities.
                </p>

                <p>
                  For example, I have a Physics system which looks for everything with a BoxCollider component, alongside Position and Velocity. It uses these to calculate physics and change the positions of the entities.
                </p>

                <p>
                  ECS is popular in games because of how simple it is, and because it's objectively faster than Object Oriented designs which require some forms of inheritance overhead.
                </p>

                <p>
                  Another large game that uses ECS is <Link href="https://www.youtube.com/watch?v=W3aieHjyNvw">Overwatch, and their team gave a talk about how they did it</Link>
                </p>
              </div>

              <div className="md:w-1/2 h-80">
                <img className="size-full object-contain" src={ecsImg} />
              </div>
            </div>
          </div>

          <div className="mt-6">
            <h3 className="text-2xl text-yellow-500 dark:text-yellow-300">- Rendering</h3>
            <hr className="my-2 w-1/2 border-black dark:border-white" />

            <div className="flex flex-col gap-2">
              <p>
                The largest part of the project was rendering abstractions. I am not fond of the OpenGL binding API, so I immediately went for the new DSA <Ref r={3} /> APIs.
              </p>

              <p>
                Atop even those, I built abstractions so that uniforms were held by index and mutated through overloads based on their type.
              </p>

              <p>
                The greatest abstraction I made was to store textures in a class <i>TextureManager</i>, which would manage a sampler2DArray, and store textures as layers in the array.
                You could then reference the textures by index, and simply set a uniform to switch between textures.
              </p>

              <p>
                This is not a new concept - this is actually the old and hacky way of having 'bindless' textures, which new graphics APIs like Vulkan <Ref r={4}/> have built in. OpenGL almost got support in for bindless textures but it kind of got dropped as Khronos moved to work on Vulkan.
              </p>

              <p>
                Finally, to render objects, I had them represented as a "Model" class, which is a C++ interface that has a draw function. This is not the most performant API since you will have 1 CPU call per model, rather than a single call for the whole scene, but it is simple enough to work for me.
              </p>
            </div>
          </div>

          <div className="mt-6">
            <h3 className="text-2xl text-yellow-500 dark:text-yellow-300">- Game Engine</h3>
            <hr className="my-2 w-1/2 border-black dark:border-white" />

            <div className="flex flex-col gap-2">
              <p>
                Midway through developing the project, I ended up structuring it as a generic game engine library.
              </p>

              <p>
                So you could easily plug things in and out of it, you can even just remove the renderer!
              </p>

              <p>
                It's heavily inspired by Bevy, check out the simple code that spins up the example Need For Speed scene.
              </p>

              <div className="bg-gray-100 max-w-full w-fit dark:bg-gray-800 px-4 py-2 rounded-md mt-2">
                <pre className="text-sm whitespace-pre-wrap max-w-full">
                  <code lang="cpp" dangerouslySetInnerHTML={{ __html: hljs.highlight(`
int main() {
  auto game = std::make_unique<Game>();
  game->addPlugin(DefaultPlugins()); // Adds default plugins like Rendering, Input, etc
  game->addPlugin(plugins::Physics()); // Adding the physics system is this easy
  game->addPlugin(scenes::nfs::NFS()); // This adds all of the systems of the NFS scene

  auto result = game->start();
  if (!result.has_value()) {
    std::println(stderr, "Failed to start game:\\n\\t{}", util::error::indent(result.error()));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
                  `, { language: "cpp" }).value}} />
                </pre>
              </div>

              <p>
                Source: <Link href="https://github.com/DvvCz/qun/blob/master/src/main.cpp">main.cpp</Link>
              </p>

              <p>
                You could even use it! (Although I wouldn't recommend doing so, I'm going to abandon the project at some point anyway..)
              </p>
            </div>
          </div>
        </div>


        <h2 className="text-3xl mt-8">Final Product</h2>
        <hr className="my-2 w-1/2 border-black dark:border-white" />

        <iframe className="mt-4 w-full h-64 md:h-[720px]"  src="https://www.youtube.com/embed/fUAYiQOhAow">
        </iframe>

        <h2 className="text-3xl mt-8">Future Work</h2>
        <hr className="my-2 w-1/2 border-black dark:border-white" />

        <div className="flex flex-col gap-2">
          <p>
            Overall, I'm pretty happy with the project. I went from practically zero OpenGL knowledge to being very comfortable with it. I feel like learning OpenGL is the equivalent of learning C as your first systems language, you have to learn where things originated from for the abstractions of other graphics APIs to make sense. Also the pain helps you appreciate them. Now, I never want to touch OpenGL again 😅
          </p>

          <p>
            For future work, I'd most definitely start from scratch using a newer graphics API. <Link href="https://wgpu.rs/">wgpu</Link> is a library I'm very interested in. It's an abstraction that can use multiple APIs as backends, ie Vulkan or WebGL.
          </p>

          <p>
            Once I'm switched to Rust and WGPU, I'd tackle rendering in multiple passes, which would allow for more performant lighting and cooler things like shadows, reflections, etc. The current forward renderer here has to loop through every single light for each fragment - which naturally tanks performance.
          </p>
        </div>

        <div className="w-full flex flex-col md:flex-row md:gap-20 md:justify-evenly">
          <div className="md:w-1/2">
            <h2 className="text-3xl mt-6">Links</h2>
            <hr className="my-2 w-full border-black dark:border-white" />

            <p>
              Here's some links that might be interesting.
            </p>

            <ul className="mt-2 list-disc pl-4">
              <li>
                <Link href="https://github.com/DvvCz/qun">
                  <FaGithub className="inline mr-2" />
                  Qun Source Code
                </Link>
              </li>
              <li>
                <Link href="https://codebycruz.com">
                  <FaGlobe className="inline mr-2" />
                  My Website
                </Link>
              </li>
              <li>
                <Link href="https://github.com/DvvCz/qun/tree/master/docs">
                  <FaBook className="inline mr-2" />
                  Source Code for this Documentation
                </Link>
              </li>
              <li>
                <Link href="https://www.youtube.com/watch?v=W3aieHjyNvw">
                  <FaCube className="inline mr-2" />
                  Overwatch ECS Talk
                </Link>
              </li>
              <li>
                <Link href="https://github.com/KhronosGroup/glTF-Sample-Assets/tree/main/Models">
                  <FaBox className="inline mr-2" />
                  glTF Sample Assets
                </Link>
              </li>
              <li>
                <Link href="https://sketchfab.com/feed">
                  <FaBox className="inline mr-2" />
                  Free 3D Assets (often glTF) from Sketchfab
                </Link>
              </li>
            </ul>
          </div>

          <div className="md:w-1/2">
            <h2 className="text-3xl mt-8">Build Instructions</h2>
            <hr className="my-2 w-full border-black dark:border-white" />

            <p>
              If you want to build the project, you'll need <Link href="https://vcpkg.io">vcpkg</Link>, <Link href="https://cmake.org">CMake</Link>, <Link href="https://ninja-build.org">Ninja</Link>, a C++ compiler supporting C++23 alongside a GPU that supports OpenGL 4.5. Then follow the steps below.
            </p>

            <ol className="mt-2 flex flex-col gap-2 list-decimal pl-4">
              <li>
                Run <code>git clone https://github.com/DvvCz/qun --recurse-submodules</code>
              </li>

              <li>
                Install dependencies with <code>vcpkg install</code>
              </li>

              <li>
                Configure the project with CMake.
              </li>

              <li>
                Build with Ninja in the /release directory
              </li>

              <li>
                Run /release/qun with cwd as the /release directory
              </li>
            </ol>
          </div>
        </div>

        <h2 className="text-3xl mt-8">Attribution</h2>
        <hr className="my-2 w-full border-black dark:border-white" />

        <p>
          All the assets provided by the project are either Public Domain / CC0, or under CC4 attribution.

          Their individual attributions can be found alongside the assets, <Link href="https://github.com/DvvCz/qun/tree/master/release/resources">here</Link>.
        </p>

        <p>
          I mostly sourced testing assets <Link href="https://github.com/KhronosGroup/glTF-Sample-Assets/tree/main/Models">from the glTF Sample Assets repository</Link>
        </p>

        <ol className="list-decimal pl-4 text-black/50 dark:text-white/50 mt-16">
          <li>
            <FootNote r={1}>
              <i>Well, OpenGL 4.5 is modern as in the latest version, but it's still about 10 years old. Some GPUs still don't bother to support it since Khronos moved on to work on Vulkan.</i>
            </FootNote>
          </li>

          <li>
            <FootNote r={2}>
              <i>Physically Based Rendering, a technique that allows for more realistic rendering of materials. It's the replacement for basic shading models like phong.</i>
            </FootNote>
          </li>

          <li>
            <FootNote r={3}>
              <i>Direct State Access, most new rendering APIs use this over the slower, less intuitive binding APIs that old OpenGL provides.</i>
            </FootNote>
          </li>

          <li>
            <FootNote r={4}>
              <i><Link href="https://www.vulkan.org/">Vulkan</Link> is a new graphics API by the same people who created OpenGL. It's far more advanced and difficult to use on its own.</i>
            </FootNote>
          </li>
        </ol>
      </div>
    </div>
  )
}

function start() {
  const root = createRoot(document.getElementById("root")!);
  root.render(<App />);
}

if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", start);
} else {
  start();
}
