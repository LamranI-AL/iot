import { supabase } from "@/hooks/useSupaBase";

async function fetchSensorData() {
  const { data, error } = await supabase
    .from("views")
    .select("*")
    .order("created_at", { ascending: false })
    .limit(10);

  if (error) {
    console.error("Erreur lors de la récupération des données :", error);
    return [];
  }

  return data;
}

export default async function Home() {
  const sensorData = await fetchSensorData();
  return (
    <div>
      <h1>Home</h1>
      {/* <BlogPage /> */}
      <div>
        <h1>Température en temps réel</h1>
        <ul>
          {sensorData?.map((data) => (
            <li key={data.id}>
              UID : {data.uid} ! - {new Date(data.created_at).toLocaleString()}
            </li>
          ))}
        </ul>
      </div>
    </div>
  );
}
