import { createClient } from "@supabase/supabase-js";

const supabaseUrl = "https://epmgkerkxgtcibpqzbht.supabase.co";
const supabaseAnonKey =
  "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImVwbWdrZXJreGd0Y2licHF6Ymh0Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3MjU2OTkzOTgsImV4cCI6MjA0MTI3NTM5OH0.NGcQ8EGkJJ0uMFpokjvcmYv5a5JIwX4Wc6qRVECPaYM";

export const supabase = createClient(supabaseUrl, supabaseAnonKey);
